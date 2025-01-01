#pragma once


#define RESET_OBJECT(obj) { if(obj) obj->Release(); obj = NULL; }

struct BOX {
    LONG x1;
    LONG y1;
    LONG x2;
    LONG y2;
};

typedef union COLOR32 {
    UINT argb;
    struct {
        BYTE b;
        BYTE g;
        BYTE r;
        BYTE a;
    };
} *PCOLOR32;

#pragma pack(push,4)
struct COLORS_DATA {
    int size;
    struct {
        COLOR32 color;
        POINT pos;
    } arr[1];
};

struct CAPTURE_DATA {
    BYTE* pBits;
    UINT Pitch;
    UINT Width;
    UINT Height;
    LONGLONG tick;
};

struct BITMAP_DATA {
    BYTE* pBits;
    LONG Pitch;
    LONG Width;
    LONG Height;
    LONG BytesPixel;
    LONG Left;
    LONG Top;
    void fillData(BYTE* dst, RECT* roi, LONG step = 0) {
        LONG linebytes, lines;
        BYTE* src;
        if (roi) {
            linebytes = (roi->right - roi->left) * BytesPixel;
            src = pBits + roi->top * Pitch + (roi->left * BytesPixel);
            lines = roi->bottom - roi->top;
        }
        else
            linebytes = Width * BytesPixel, lines = Height, src = pBits;
        if(step<linebytes)step=linebytes;
        if (Pitch == linebytes && Pitch == step)
            memcpy(dst, src, Pitch * lines);
        else {
            for (int i = 0; i < lines; i++)
                memcpy(dst, src, linebytes), dst += step, src += Pitch;
        }
    }
    void updateDesc(BITMAP_DATA* src, LONG pitch, LONG bytes) {
        Width = src->Width;
        Height = src->Height;
        Left = src->Left;
        Top = src->Top;
        Pitch = pitch;
        BytesPixel = bytes;
    }
    void create(int width, int height, int bytes) {
        Pitch = width * bytes;
        Width = width;
        Height = height;
        BytesPixel = bytes;
        pBits = (BYTE*)malloc(Pitch * Height);
    }
    ~BITMAP_DATA() {
        if (pBits)
            free(pBits);
    }
};

#pragma pack(pop,4)
