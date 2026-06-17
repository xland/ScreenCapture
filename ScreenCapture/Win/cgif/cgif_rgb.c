#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "cgif.h"

#define MULU16(a, b) (((uint32_t)a) * ((uint32_t)b)) // helper macro to correctly multiply two U16's without default signed int promotion
#define MAX(a,b) (a >= b ? a : b)
#define MIN(a,b) (a <= b ? a : b)
#define MAX_TABLE_SIZE 16777216 // 256*256*256
#define MAX_COLLISIONS 30 // maximum number of collisions before hash-table resize

struct st_cgif_rgb {
  CGIF*          pGIF;
  CGIFrgb_Config config;
  uint8_t*       pBefImageData;
  cgif_chan_fmt  befFmtChan;
  cgif_result    curResult;
};

typedef struct {
  uint8_t r,g,b,a;
} Pixel;

typedef struct treeNode {
  struct treeNode* child0; // pointer to child-node (elements smaller or equal than mean)
  struct treeNode* child1; // pointer to child-node (elements larger than mean)
  float mean[3];           // average of the colors
  uint32_t idxMin, idxMax; // minimum and maximum index referring to global palette
  uint8_t cutDim;          // dimension along which the cut (node split) is done
  uint8_t colIdx;          // color index (only meaningful for leave node)
  bool isLeave;            // is leave node or not
} treeNode;

typedef struct colHashTable {
  uint32_t* frequ;     // frequency of colors (histogram)
  uint8_t* hashTable;  // stores RGB vales at position determined by hash
  uint8_t* indexUsed;  // which part of the hash table was already used
  uint8_t* pPalette;   // color palette with all the colors (all in one block)
  uint32_t* colIdx;    // index of the color (mapping from hash to index in pPalette)
  uint32_t  cnt;       // count the number of colors
  uint32_t  tableSize; // initial size of the hash table
} colHashTable;

static uint64_t argmax64(float* arry, uint64_t n){
  uint64_t imax = 0;
  float vmax = 0;

  for(uint64_t i = 0; i < n; ++i) {
    if(arry[i] > vmax){
      imax = i;
      vmax = arry[i];
    }
  }
  return imax;
}

/* get the next prime number above the next power of two */
static int getNextPrimePower2(int N){
  if(N <= 512) {return 521;}
  else if(N <= 1024) {return 1031;}
  else if (N <= 2048) {return 2053;}
  else if (N <= 4096) {return 4099;}
  else if (N <= 8192) {return 8209;}
  else if (N <= 16384) {return 16411;}
  else if (N <= 32768) {return 32771;}
  else if (N <= 65536) {return 65537;}
  else if (N <= 131072) {return 131101;}
  else if (N <= 262144) {return 262147;}
  else if (N <= 524288) {return 524309;}
  else if (N <= 1048576) {return 1048583;}
  else if (N <= 2097152) {return 2097169;}
  else if (N <= 4194304) {return 4194319;}
  else if (N <= 8388608) {return 8388617;}
  else {return MAX_TABLE_SIZE;} // largest table size that is meaningful (no prime number)
}

static Pixel getPixelVal(const uint8_t* pData, cgif_chan_fmt fmtChan) {
  Pixel result;

  result.r = *(pData++);
  result.g = *(pData++);
  result.b = *(pData++);
  // check whether there is a alpha channel, if so convert it to a 1 bit alpha channel.
  switch(fmtChan) {
  case CGIF_CHAN_FMT_RGB:
    result.a = 0xFF;
    break;
  case CGIF_CHAN_FMT_RGBA:
    result.a = (*(pData) <= 127) ? 0 : 0xFF;
    break;
  }
  return result;
}

/* get a hash from rgb color values (use open addressing, if entry does not exist, next free spot is returned) */
static int32_t col_hash(const uint8_t* rgb, const uint8_t* hashTable, const uint8_t* indexUsed, const uint32_t tableSize, cgif_chan_fmt fmtChan) {
  uint32_t h;
  const Pixel pix = getPixelVal(rgb, fmtChan);
  if(pix.a == 0) { // has alpha channel?
    return -1;
  }
  h = (pix.r * 256 * 256 + pix.g * 256 + pix.b) % tableSize;
  while(indexUsed[h] != 0 && memcmp(rgb, &hashTable[3 * h], 3) != 0) { // until free spot is found
    ++h;
    h = h % tableSize; // start from beginning if there is no free spot at the end
  }
  return h;
}

/* same as col_hash but also count the number of collisions */
static int32_t col_hash_collision_count(const uint8_t* rgb, const uint8_t* hashTable, const uint8_t* indexUsed, const uint32_t tableSize, cgif_chan_fmt fmtChan, uint32_t* cntCollision) {
  uint32_t h;
  const Pixel pix = getPixelVal(rgb, fmtChan);
  *cntCollision = 0; // count the number of collisions
  if(pix.a == 0) { // has alpha channel?
    return -1;
  }
  h = (pix.r * 256 * 256 + pix.g * 256 + pix.b) % tableSize;
  while(indexUsed[h] != 0 && memcmp(rgb, &hashTable[3 * h], 3) != 0) { // until free spot is found
    ++(*cntCollision);
    ++h;
    h = h % tableSize; // start from beginning if there is no free spot at the end
  }
  return h;
}

/* free the color hash table*/
static void free_col_hash_table(colHashTable* colhash){
  free(colhash->frequ);
  free(colhash->hashTable);
  free(colhash->indexUsed);
  free(colhash->pPalette);
  free(colhash->colIdx);
  free(colhash);
}

/* initialize hash table storing colors and their frequency */
static colHashTable* init_col_hash_table(uint32_t tableSize){
  colHashTable* colhash = malloc(sizeof(colHashTable));
  if(colhash == NULL) {
    return NULL;
  }
  memset(colhash, 0, sizeof(colHashTable));
  colhash->tableSize = getNextPrimePower2(tableSize); // increase table size to next prime number
  colhash->frequ = malloc(sizeof(uint32_t) * colhash->tableSize);
  colhash->hashTable = malloc(3 * colhash->tableSize);
  colhash->indexUsed = malloc(colhash->tableSize);
  colhash->pPalette = malloc(3 * colhash->tableSize);
  colhash->colIdx = malloc(sizeof(uint32_t)*colhash->tableSize);
  if(!colhash->frequ || !colhash->hashTable || !colhash->indexUsed || !colhash->pPalette || !colhash->colIdx) {
    free_col_hash_table(colhash);
    return NULL;
  }
  colhash->cnt = 0;                                      // no colors initially
  memset(colhash->pPalette, 0, 3 * colhash->tableSize);  // unused part of color table is uninitialized otheriwse
  memset(colhash->indexUsed, 0, colhash->tableSize);     // initially no entry in hash-table is used
  return colhash;
}

/* increase the size of the color hash table */
static int resize_col_hash_table(colHashTable* colhash){
  uint32_t tableSizeNew;
  tableSizeNew  = getNextPrimePower2(colhash->tableSize); // increase table size to the next prime number above the next power of two
  uint8_t* pPaletteNew   = realloc(colhash->pPalette, 3 * tableSizeNew);
  uint32_t* colIdxNew    = realloc(colhash->colIdx, sizeof(uint32_t) * tableSizeNew);
  uint8_t* hashTable_new = malloc(3 * tableSizeNew);
  uint8_t* indexUsed_new = malloc(tableSizeNew);
  uint32_t* frequ_new    = malloc(sizeof(uint32_t) * tableSizeNew);
  if(!pPaletteNew || !colIdxNew || !hashTable_new || !indexUsed_new || !frequ_new) {
    // On allocation failure, apply any successful reallocs to colhash and free
    // any newly allocated auxiliary buffers before returning an error.
    if(pPaletteNew) colhash->pPalette = pPaletteNew;
    if(colIdxNew)   colhash->colIdx   = colIdxNew;
    free(hashTable_new);
    free(indexUsed_new);
    free(frequ_new);
    return -1;
  }
  colhash->pPalette = pPaletteNew;
  colhash->colIdx   = colIdxNew;
  memset(indexUsed_new, 0, tableSizeNew);
  colhash->cnt = 0;
  for(uint32_t j = 0; j < colhash->tableSize; ++j) { // TBD (no improvement when tested): easier to loop over pPalette and also leave pPalette in place?, if indexUsed is also unnecessary then
    if(colhash->indexUsed[j] == 1){
      uint32_t h = col_hash(colhash->hashTable + 3 * j, hashTable_new, indexUsed_new, tableSizeNew, 3); // recompute hash with new table size
      memcpy(&hashTable_new[3 * h], colhash->hashTable + 3 * j, 3); // put value from old hash table to right position of the new one
      memcpy(colhash->pPalette + 3 * colhash->cnt, colhash->hashTable + 3 * j, 3);
      indexUsed_new[h] = 1;
      frequ_new[h] = colhash->frequ[j];
      colhash->colIdx[h] = colhash->cnt;
      ++(colhash->cnt);
    }
  }
  colhash->tableSize = tableSizeNew;
  free(colhash->hashTable); // free part of old hash table that is not used anymore
  free(colhash->indexUsed); // free part of old hash table that is not used anymore
  free(colhash->frequ); // free part of old hash table that is not used anymore
  colhash->hashTable = hashTable_new; // pass pointer to new hash table
  colhash->indexUsed = indexUsed_new; // pass pointer to new hash table
  colhash->frequ = frequ_new; // pass pointer to new hash table
  return 0;
}

/* take frequ indexed by hash(rgb) and return corresponding dense array */
static uint32_t* hash_to_dense(colHashTable* colhash, cgif_chan_fmt fmtChan) {
  uint32_t* frequDense = malloc(sizeof(uint32_t) * colhash->cnt);
  uint32_t h;
  (void)fmtChan;
  if(frequDense == NULL) {
    return NULL;
  }
  for(uint32_t i = 0; i < colhash->cnt; ++i) {
    h = col_hash(colhash->pPalette + 3 * i, colhash->hashTable, colhash->indexUsed, colhash->tableSize, 3);
    frequDense[i] = colhash->frequ[h];
  }
  return frequDense;
}

/* get mean of color-cloud along all 3 dimensions (at least one color must be present, otherwise div 0 issue)*/
static void get_mean(const uint8_t* pPalette, const uint32_t* frequ, uint32_t idxMin, uint32_t idxMax, float* mean){
  // pPalette: RGB color palette
  // frequ: frequency of the colors (indexing as pPalette, no hashing)
  // idxMin, idxMax: palette range for which the mean is computed
  // mean: mean value along all three dimensions
  float m[3]   = {0,0,0};
  float sum[3] = {0,0,0};
  uint32_t i;
  uint8_t dim;

  for(i = idxMin; i <= idxMax; ++i) {
    for(dim = 0; dim < 3; ++dim) {
      sum[dim] += frequ[i];
      m[dim]   += frequ[i] * pPalette[3 * i + dim];
    }
  }
  for(dim = 0; dim < 3; ++dim) {
    mean[dim] = m[dim] / sum[dim];
  }
}

/* get variance of color-cloud along all 3 dimensions*/
static void get_variance(const uint8_t* pPalette, const uint32_t* frequ, uint32_t idxMin, uint32_t idxMax, float* var, float* mean){
  // pPalette: RGB color palette
  // frequ: frequency of the colors (indexing as pPalette, no hashing)
  // idxMin, idxMax: palette range for which the variance is computed
  // var/mean: variance/mean value along all three dimensions (array with 3 entries)
  uint32_t i;
  uint8_t dim;
  float v[3] = {0,0,0};
  float sum[3] = {0,0,0};

  get_mean(pPalette, frequ, idxMin, idxMax, mean);
  for(i = idxMin; i <= idxMax; ++i) {
    for(dim = 0; dim < 3; ++dim) {
      sum[dim] += frequ[i];
      v[dim]   += frequ[i] * (pPalette[3 * i + dim] - mean[dim]) * (pPalette[3 * i + dim] - mean[dim]);
    }
  }
  for(dim = 0; dim < 3; ++dim) {
    var[dim] = v[dim] / sum[dim];
  }
}

static treeNode* new_tree_node(uint8_t* pPalette, uint32_t* frequ, uint16_t* numLeaveNodes, uint32_t idxMin, uint32_t idxMax, uint8_t colIdx) {
  float var[3];

  treeNode* node = malloc(sizeof(treeNode));
  if(node == NULL) {
    return NULL;
  }
  node->idxMin   = idxMin; // minimum color in pPalette belonging to the node
  node->idxMax   = idxMax; // maximum color in pPalette belonging to the node
  get_variance(pPalette, frequ, idxMin, idxMax, var, node->mean);
  node->cutDim  = argmax64(var, 3); // dimension along which the cut (node split) is done
  node->colIdx  = colIdx; // index referring to (averaged) color in new color table
  node->isLeave = 1;      // every new node starts as a leave node
  (*numLeaveNodes)++;     // increase counter when leave node is added
  return node;
}

/* create the decision tree. (Similar to qsort with limited depth: pPalette, frequ get sorted) */
static int crawl_decision_tree(treeNode* root, uint16_t* numLeaveNodes, uint8_t* pPalette, uint32_t* frequ, uint16_t colMax) {
  uint32_t i, k, saveNum;
  uint16_t nodeIdx = 0;
  uint8_t saveBlk[3];
  treeNode* parent;
  treeNode* nodeList[512]; // array of pointers to nodes for breadth-first tree traversal
  nodeList[0] = root;

  while(*numLeaveNodes <= (colMax - 1)){
    parent = nodeList[nodeIdx++]; // go to next node in the list
    if (parent->idxMax - parent->idxMin >= 1) { // condition for node split
    i = parent->idxMin; // start of block minimum
    k = parent->idxMax; // start at block maximum
    while(i < k) { // split parent node in two blocks (like one step in qsort)
      for(; pPalette[3 * i + parent->cutDim] <= parent->mean[parent->cutDim]; ++i); // && i<parent->idxMax not needed (other condition is false when i==parent>idxMax since there must be at most 1 element above mean)
      for(; pPalette[3 * k + parent->cutDim] > parent->mean[parent->cutDim];  --k); // && k>parent->idxMin not needed (other condition is false when k==parent>idxMin since there must be at most 1 element below mean)
      if(k > i) {
        memcpy(saveBlk, &(pPalette[3 * i]), 3);
        memcpy(&(pPalette[3 * i]), &(pPalette[3 * k]), 3); // swap RGB-blocks in pPalette
        memcpy(&(pPalette[3 * k]), saveBlk, 3);            // swap RGB-blocks in pPalette
        saveNum  = frequ[k];
        frequ[k] = frequ[i]; // swap also the frequency
        frequ[i] = saveNum;  // sawp also the frequency
      }
    }
    parent->isLeave = 0; // parent is no leave node anymore when children added
    (*numLeaveNodes)--;  // decrease counter since parent is removed as a leave node
    parent->child0 = new_tree_node(pPalette, frequ, numLeaveNodes, parent->idxMin, i - 1, parent->colIdx); // i-1 is last index of 1st block, one child takes color index from parent
    parent->child1 = new_tree_node(pPalette, frequ, numLeaveNodes, i, parent->idxMax, *numLeaveNodes);
    if(parent->child0 == NULL || parent->child1 == NULL) {
      return -1;
    }
    nodeList[2*(*numLeaveNodes) - 3] = parent->child0; // add new child nodes to the list (total number of nodes is always 2*(*numLeaveNodes)-1)
    nodeList[2*(*numLeaveNodes) - 2] = parent->child1; // add new child nodes to the list (total number of nodes is always 2*(*numLeaveNodes)-1)
    }
  }
  return 0;
}

/* fill 256 color table using the decision tree */
static void get_palette_from_decision_tree(const treeNode* root, uint8_t* pPalette256){
  if(root->isLeave == 0) {
    get_palette_from_decision_tree(root->child0, pPalette256);
    get_palette_from_decision_tree(root->child1, pPalette256);
  } else {
    pPalette256[3 * root->colIdx]     = (uint8_t)roundf(root->mean[0]);
    pPalette256[3 * root->colIdx + 1] = (uint8_t)roundf(root->mean[1]);
    pPalette256[3 * root->colIdx + 2] = (uint8_t)roundf(root->mean[2]);
  }
}

/* get color index by using the decision tree */
static uint8_t get_leave_node_index(const treeNode* root, const float* rgb) {
  if(root->isLeave == 0) { // if there is a split
    if(rgb[root->cutDim] <= root->mean[root->cutDim]) {
      return get_leave_node_index(root->child0, rgb);
    } else {
      return get_leave_node_index(root->child1, rgb);
    }
  } else {
    return root->colIdx; // return color index of leave node
  }
}

/* free memory allocated for the tree */
static void free_decision_tree(treeNode* root){
  if(root == NULL) {
    return;
  }
  if(root->isLeave == 0) { // if the node has children
    free_decision_tree(root->child0);
    free_decision_tree(root->child1);
  }
  free(root); // free root once free is called for children
}

/* color quantization with mean cut method (TBD? switch to median cut)
   (works with dense palette, no hash table) */
static treeNode* create_decision_tree(uint8_t* pPalette,  uint32_t* pFrequDense, uint8_t* pPalette256, uint32_t cnt, uint16_t colMax, uint8_t depthMax){
  uint16_t numLeaveNodes = 0;
  treeNode* root = new_tree_node(pPalette, pFrequDense, &numLeaveNodes, 0, cnt - 1, 0);
  if(root == NULL) {
    return NULL;
  }
  if(crawl_decision_tree(root, &numLeaveNodes, pPalette, pFrequDense, colMax) != 0) {
    free_decision_tree(root);
    return NULL;
  }
  get_palette_from_decision_tree(root, pPalette256); // fill the reduced color table
  return root;
}

/* get image with max 256 color indices using Floyd-Steinberg dithering */
static void get_quantized_dithered_image(uint8_t* pImageData, float* pImageDataRGBfloat, uint8_t* pPalette256, treeNode* root, uint32_t numPixel, uint32_t width, uint8_t dithering, uint8_t transIndex, cgif_chan_fmt fmtChan, uint8_t* pBef, cgif_chan_fmt befFmtChan, int hasAlpha) {
  // pImageData: image with (max 256) color indices (length: numPixel)
  // pImageDataRGBfloat: image with RGB colors (length: 3*numPixel) must be signed to avoid overflow due to error passing, float only needed because of 0.9 factor
  // pPalette256: quantized color palette (indexed by node->colIdx), only used if dithering is on
  // root: root node of the decision tree for color quantization
  // numPixel, width: size of the image
  // dithering: 0 (no dithering), 1: Floyd-Steinberg dithering, else: Sierra dithering
  uint32_t i;
  uint8_t k;
  int err; // color errors
  /*
    we noticed certain artifacts with Floyd-Steinberg dithering if the error is passed too far.
    use a damping factor to reduce these effects.
  */
  const double factor = 0.90; // Has to be a double to force double-precision arithmetic - otherwise we might get rounding specific differences between platforms (eg. macOS Clang)
  if(!dithering) {
    for(i = 0; i < numPixel; ++i) {
      if(hasAlpha) {
        if(pImageDataRGBfloat[fmtChan * i + 3] == 0) {
          pImageData[i] = transIndex;
          continue;
        }
      } else {
        // do the transparency trick
        float* p = pImageDataRGBfloat;
        if(pBef && p[i * fmtChan] == pBef[i * befFmtChan] && p[i * fmtChan + 1] == pBef[i * befFmtChan + 1] && p[i * fmtChan + 2] == pBef[i * befFmtChan + 2]) {
          pImageData[i] = transIndex;
          continue;
        }
      }
      pImageData[i] = get_leave_node_index(root, &pImageDataRGBfloat[fmtChan * i]);  // use decision tree to get indices for new colors
    }
  } else {
    for(i = 0; i < numPixel; ++i) {
      if(fmtChan == CGIF_CHAN_FMT_RGBA) {
        if(pImageDataRGBfloat[fmtChan * i + 3] == 0) {
          pImageData[i] = transIndex;
          continue;
        }
      }
      // TBD add the transparency trick

      // restrict color + error to 0-255 interval
      for(k = 0; k<3; ++k) {
        pImageDataRGBfloat[fmtChan * i + k] = MAX(0,MIN(pImageDataRGBfloat[fmtChan * i + k], 255)); // cut to 0-255 before
      }

      pImageData[i] = get_leave_node_index(root, &pImageDataRGBfloat[fmtChan * i]);  // use decision tree to get indices for new colors
      for(k = 0; k<3; ++k) {
        err = pImageDataRGBfloat[fmtChan * i + k] - pPalette256[3 * pImageData[i] + k]; // compute color error

        //diffuse error with Floyd-Steinberg dithering.
        if(dithering == 1) {
          if(i % width < width-1){
            pImageDataRGBfloat[fmtChan * (i+1) + k] += factor * (7*err >> 4);
          }
          if(i < numPixel - width){
            pImageDataRGBfloat[fmtChan * (i+width) + k] += factor * (5*err >> 4);
            if(i % width > 0){
              pImageDataRGBfloat[fmtChan * (i+width-1) + k] += factor * (3*err >> 4);
            }
            if(i % width < width-1){
              pImageDataRGBfloat[fmtChan * (i+width+1) + k] += factor * (1*err >> 4);
            }
          }
        } else {
          // Sierra dithering
          if(i % width < width-1){
            pImageDataRGBfloat[fmtChan * (i+1) + k] += factor * (5*err >> 5);
            if(i % width < width-2){
              pImageDataRGBfloat[fmtChan * (i+2) + k] += factor * (3*err >> 5);
            }
          }
          if(i < numPixel - width){
            pImageDataRGBfloat[fmtChan * (i+width) + k] += factor * (5*err >> 5);
            if(i % width > 0){
              pImageDataRGBfloat[fmtChan * (i+width-1) + k] += factor * (4*err >> 5);
              if(i % width > 1){
                pImageDataRGBfloat[fmtChan * (i+width-2) + k] += factor * (2*err >> 5);
              }
            }
            if(i % width < width-1){
              pImageDataRGBfloat[fmtChan * (i+width+1) + k] += factor * (4*err >> 5);
              if(i % width < width-2){
                pImageDataRGBfloat[fmtChan * (i+width+2) + k] += factor * (2*err >> 5);
              }
            }
            if(i < numPixel - 2*width){
              pImageDataRGBfloat[fmtChan * (i+2*width) + k] += factor * (3*err >> 5);
              if(i % width > 0){
                pImageDataRGBfloat[fmtChan * (i+2*width-1) + k] += factor * (2*err >> 5);
              }
              if(i % width < width-1){
                pImageDataRGBfloat[fmtChan * (i+2*width+1) + k] += factor * (2*err >> 5);
              }
            }
          }
        }
      }
    }
  }
}

/* get color histogram of and rgb image with up to 256*256*256 colors */
// pImageDataRGB: image in RGB format
// numPixel: number of pixels of input image
// fmtChan: select if image data are RGB or RGBa
// pHasAlpha: is alpha channel present
static colHashTable* get_color_histogram(const uint8_t* pImageDataRGB, uint32_t numPixel, cgif_chan_fmt fmtChan, int* pHasAlpha) {
  uint32_t cntCollision;                                    // count the number of collision
  uint32_t  tableSize = 262147;                             // initial size of the hash table
  colHashTable* colhash = init_col_hash_table(tableSize);   // initialize the hash table storing all the colors
  if(colhash == NULL) {
    return NULL;
  }
  *pHasAlpha = 0;                                           // assume no alpha channel until it is found
  const uint8_t sizePixel = fmtChan;                        // number of bytes for one pixel (e.g. 3 for RGB, 4 for RGBa)
  for(uint32_t i = 0; i < numPixel; ++i) {
    uint32_t h = col_hash_collision_count(&pImageDataRGB[sizePixel * i], colhash->hashTable, colhash->indexUsed, colhash->tableSize, fmtChan, &cntCollision);
    if(h == -1) { // -1 means that the user has set a user-defined transparency (alpha channel)
      *pHasAlpha = 1;
      continue; // do not take this pixel into account (e.g. alpha channel present)
    }
    if(colhash->indexUsed[h] == 0) {
      memcpy(colhash->hashTable + 3 * h, &pImageDataRGB[sizePixel * i], 3); // add new color to hash table
      memcpy(colhash->pPalette + 3 * colhash->cnt, &pImageDataRGB[sizePixel * i], 3); // add new color to palette
      colhash->indexUsed[h] = 1;             // mark hash table entry as used
      colhash->colIdx[h]    = colhash->cnt;  // assign index to color
      colhash->frequ[h]     = 1;             // the new color occured 1x (1st time)
      ++(colhash->cnt);                      // one new color added
    } else {                                 // if color exists already
      colhash->frequ[h] += 1;                // increment color histrogram
    }
    // resize the hash table (if more than half-full)
    if((colhash->cnt > (colhash->tableSize >> 1) || cntCollision > MAX_COLLISIONS) && colhash->tableSize < MAX_TABLE_SIZE) {
      if(resize_col_hash_table(colhash) != 0) {
        free_col_hash_table(colhash);
        return NULL;
      }
    }
  }
  return colhash;
}

/* quantize the image using the color histogram */
static int quantize_and_dither(colHashTable* colhash, const uint8_t* pImageDataRGB, uint32_t numPixel, uint32_t width, cgif_chan_fmt fmtChan, uint8_t* pImageData, uint8_t* pPalette256, uint8_t depthMax, uint8_t dithering, int hasAlpha, uint8_t* pBef, cgif_chan_fmt befFmtChan) {
  // pImageDataRGB: image in RGB format
  // numPixel: number of pixels of input image
  // width: width of the image (needed for dithering)
  // fmtChan: select if image data are RGB or RGBa
  // pImageData: pointer to image as indices (output)
  // pPalette256: new color palette with 256 colors max.
  // depthMax: maximum depth of decision tree for colors quantization (sets number of colors)
  // dithering: 0 (no dithering), 1: Floyd-Steinberg dithering, else: Sierra dithering
  // hasAlpha: alpha channel present 1, 0 otheriwse
  // pBef, befFmtChan
  const uint8_t sizePixel = fmtChan; // number of bytes for one pixel (e.g. 3 for RGB, 4 for RGBa)
  const uint16_t colMax = (1uL << depthMax) - 1; // maximum number of colors (-1 to leave space for transparency), disadvantage (TBD): quantization for static image with 256 colors and no alpha channel unnecessary
  if(colhash->cnt > colMax) { // color-quantization is needed
    uint32_t* pFrequDense = hash_to_dense(colhash, fmtChan);
    if(pFrequDense == NULL) {
      return -1;
    }
    treeNode* root        = create_decision_tree(colhash->pPalette, pFrequDense, pPalette256, colhash->cnt, colMax, depthMax); // create decision tree (dynamic, splits along rgb-dimension with highest variance)
    free(pFrequDense);
    if(root == NULL) {
      return -1;
    }
    float* pImageDataRGBfloat = malloc(fmtChan * numPixel * sizeof(float)); // TBD fmtChan + only when hasAlpha
    if(pImageDataRGBfloat == NULL) {
      free_decision_tree(root);
      return -1;
    }
    for(uint32_t i = 0; i < fmtChan * numPixel; ++i){
      pImageDataRGBfloat[i] = pImageDataRGB[i];
    }
    uint8_t transIndex = colMax;
    get_quantized_dithered_image(pImageData, pImageDataRGBfloat, pPalette256, root, numPixel, width, dithering, transIndex, fmtChan, pBef, befFmtChan, hasAlpha); // do color quantization and dithering
    free(pImageDataRGBfloat); // RGB image is not needed anymore
    free_decision_tree(root); // tree for color quantization is not needed anymore
    colhash->cnt = colMax;
  } else { // no color-quantization is needed if the number of colors is small enough
    uint8_t transIndex = colhash->cnt;
    for(uint32_t i = 0; i < numPixel; ++i) {
      if(!hasAlpha && pBef) {
        if(memcmp(&pImageDataRGB[sizePixel * i], &pBef[befFmtChan * i], 3) == 0) {
          pImageData[i] = transIndex;
          continue;
        }
      }
      uint32_t h = col_hash(&pImageDataRGB[sizePixel * i], colhash->hashTable, colhash->indexUsed, colhash->tableSize, fmtChan);
      pImageData[i] = (h == -1) ? transIndex : colhash->colIdx[h];
    }
    memcpy(pPalette256, colhash->pPalette, 3 * 256); // keep the color palette (no quantization)
  }
  return colhash->cnt; // return number of colors found
}

CGIFrgb* cgif_rgb_newgif(const CGIFrgb_Config* pConfig) {
  CGIF_Config idxConfig = {0};
  CGIFrgb* pGIFrgb;
  
  pGIFrgb = malloc(sizeof(CGIFrgb));
  if(pGIFrgb == NULL) {
    return NULL;
  }
  memset(pGIFrgb, 0, sizeof(CGIFrgb));
  idxConfig.pWriteFn  = pConfig->pWriteFn;
  idxConfig.pContext  = pConfig->pContext;
  idxConfig.path      = pConfig->path;
  idxConfig.numLoops  = pConfig->numLoops;
  idxConfig.width     = pConfig->width;
  idxConfig.height    = pConfig->height;
  idxConfig.attrFlags = CGIF_ATTR_IS_ANIMATED | CGIF_ATTR_NO_GLOBAL_TABLE;
  pGIFrgb->pGIF       = cgif_newgif(&idxConfig);
  if(pGIFrgb->pGIF == NULL) {
    free(pGIFrgb);
    return NULL;
  }
  pGIFrgb->config    = *pConfig;
  pGIFrgb->curResult = CGIF_PENDING;
  return pGIFrgb;  
}

cgif_result cgif_rgb_addframe(CGIFrgb* pGIF, const CGIFrgb_FrameConfig* pConfig) {
  uint8_t          aPalette[256 * 3];
  memset(aPalette, 0, 256 * 3);       // quantize_and_dither does not necessarily fill/use all palette entries (-> initialize aPalette to avoid underfined behaviour)
  CGIF_FrameConfig fConfig     = {0};
  uint8_t* pNewBef;
  const uint16_t   imageWidth  = pGIF->config.width;
  const uint16_t   imageHeight = pGIF->config.height;
  const uint32_t   numPixel    = MULU16(imageWidth, imageHeight);
  int              hasAlpha;
  // check for previous errors
  if(pGIF->curResult != CGIF_OK && pGIF->curResult != CGIF_PENDING) {
    return pGIF->curResult;
  }
  // check if fmtChan param is valid (only RGB, RGBA allowed)
  if(pConfig->fmtChan != CGIF_CHAN_FMT_RGB && pConfig->fmtChan != CGIF_CHAN_FMT_RGBA) {
    pGIF->curResult = CGIF_ERROR;
    return CGIF_ERROR;
  }
  pNewBef = malloc(pConfig->fmtChan * MULU16(imageWidth, imageHeight));
  if(pNewBef == NULL) {
    pGIF->curResult = CGIF_EALLOC;
    return pGIF->curResult;
  }
  memcpy(pNewBef, pConfig->pImageData, pConfig->fmtChan * MULU16(imageWidth, imageHeight));
  fConfig.pLocalPalette = aPalette;
  fConfig.pImageData    = malloc(pGIF->config.width * (uint32_t)pGIF->config.height);
  if(fConfig.pImageData == NULL) {
    free(pNewBef);
    pGIF->curResult = CGIF_EALLOC;
    return pGIF->curResult;
  }
  fConfig.delay         = pConfig->delay;
  fConfig.attrFlags     = CGIF_FRAME_ATTR_USE_LOCAL_TABLE;
  if(pConfig->attrFlags & CGIF_RGB_FRAME_ATTR_INTERLACED) {
    fConfig.attrFlags |= CGIF_FRAME_ATTR_INTERLACED;
  }

  colHashTable* colhash = get_color_histogram(pConfig->pImageData, numPixel, pConfig->fmtChan, &hasAlpha);
  if(colhash == NULL) {
    free(pNewBef);
    free(fConfig.pImageData);
    pGIF->curResult = CGIF_EALLOC;
    return pGIF->curResult;
  }
  const uint8_t bDither = !(pConfig->attrFlags & CGIF_RGB_FRAME_ATTR_NO_DITHERING);
  const int sizeLCT = quantize_and_dither(colhash, pConfig->pImageData, numPixel, pGIF->config.width, pConfig->fmtChan, fConfig.pImageData, aPalette, 8, bDither, hasAlpha, pGIF->pBefImageData, pGIF->befFmtChan);
  free_col_hash_table(colhash);
  if(sizeLCT < 0) {
    free(pNewBef);
    free(fConfig.pImageData);
    pGIF->curResult = CGIF_EALLOC;
    return pGIF->curResult;
  }

  fConfig.numLocalPaletteEntries = sizeLCT;
  if(hasAlpha) {
    fConfig.attrFlags   |= CGIF_FRAME_ATTR_HAS_ALPHA;
    fConfig.transIndex   = sizeLCT;
  } else {
    fConfig.attrFlags |= CGIF_FRAME_ATTR_HAS_SET_TRANS;
    fConfig.transIndex = sizeLCT;
  }
  cgif_result r = cgif_addframe(pGIF->pGIF, &fConfig);
  free(pGIF->pBefImageData);
  pGIF->pBefImageData = pNewBef;
  pGIF->befFmtChan    = pConfig->fmtChan;

  // cleanup
  free(fConfig.pImageData);
  pGIF->curResult = r;
  return r;
}

cgif_result cgif_rgb_close(CGIFrgb* pGIF) {
  cgif_result r = cgif_close(pGIF->pGIF);
  free(pGIF->pBefImageData);
  free(pGIF);
  return r;
}
