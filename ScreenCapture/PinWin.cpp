#include "PinWin.h"
#include "History.h"

PinWin::PinWin(const int& x, const int& y, BLImage* img)
{
    state = State::maskReady;    
	this->x = x-16;
	this->y = y-16;
	this->w = img->width() + 32;
    if (w < toolBoxWidth+32) {
        this->w = toolBoxWidth+32;
    }
	this->h = img->height() + 32 + 2*toolBoxHeight + 2*toolBoxSpan;
    stride = w * 4;
    dataSize = stride * h;
    //-------------------------------------------
    InitLayerImg();
    OriginalImage = img;
    cutBox.x0 = 16;
    cutBox.y0 = 16;
    cutBox.x1 = 16 + img->width();
    cutBox.y1 = 16 + img->height();
    IsMainWin = false;
	InitWindow();
	Show();    
}
PinWin::~PinWin()
{

}

void PinWin::SaveFile(const std::string& filePath) {
    auto w = cutBox.x1 - cutBox.x0;
    auto h = cutBox.y1 - cutBox.y0;
    BLImage imgSave(w, h, BL_FORMAT_PRGB32);
    PaintCtx->begin(imgSave);
    PaintCtx->blitImage(BLPoint(0, 0), *OriginalImage, BLRectI(0, 0, (int)w, (int)h));
    PaintCtx->blitImage(BLPoint(0, 0), *CanvasImage, BLRectI(16, 16, (int)w, (int)h));
    PaintCtx->end();
    imgSave.writeToFile(filePath.c_str());
}

void PinWin::BeforePaint() {
    PaintCtx->begin(*BottomImage);
    PaintCtx->clearAll();
    drawShadow();
    BLPoint startPos(16, 16);
    BLRectI srcRect(16, 16, OriginalImage->width(), OriginalImage->height());
    PaintCtx->blitImage(startPos, *OriginalImage);
    if (IsMosaicUsePen) {
        PaintCtx->blitImage(startPos, *MosaicImage, srcRect);
    }
    else
    {
        PaintCtx->blitImage(startPos, *CanvasImage,srcRect);
    }
    if (IsDrawing) {
        PaintCtx->blitImage(startPos, *PrepareImage,srcRect);
    }
    setToolBoxPos();
    drawToolMain();
    PaintCtx->end();
}

int PinWin::OnHitTest(const int& x, const int& y) {
    if (!OriginalImage) {
        return HTNOWHERE;
    }
    if (checkMouseEnterToolBox(x-this->x, y-this->y)) {
        return HTCLIENT;
    }
    if (x > this->x + 16 && x<this->x + OriginalImage->width() + 16 && y>this->y + 16 && y < this->y + OriginalImage->height() + 16) {
        if (state == State::maskReady) {
            ChangeCursor(IDC_SIZEALL);
            return HTCAPTION;
        }
        else
        {
            return HTCLIENT;
        }
        
    }
    return HTNOWHERE;
}

void PinWin::setToolBoxPos()
{
    toolBoxMain.x0 = 16;
    toolBoxMain.y0 = OriginalImage->height() + 32;
    toolBoxMain.y1 = toolBoxMain.y0 + toolBoxHeight;
    toolBoxMain.x1 = toolBoxMain.x0 + toolBoxWidth;
}

void PinWin::drawShadow()
{
    unsigned w = OriginalImage->width();
    unsigned h = OriginalImage->height();
    {//左上
        BLGradient radial(BLRadialGradientValues(16, 16, 16, 16, 16));
        radial.addStop(0.0, BLRgba32(0x22000000));
        radial.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillCircle(16, 16, 16, radial);

        PaintCtx->setCompOp(BL_COMP_OP_CLEAR);
        PaintCtx->setFillStyle(BLRgba32(0xFF000000));
        PaintCtx->fillBox(16, 0, 32, 16);
        PaintCtx->fillBox(0, 16, 16, 32);
        PaintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    }
    {//右上
        BLGradient radial(BLRadialGradientValues(w + 16, 16, w + 16, 16, 16));
        radial.addStop(0.0, BLRgba32(0x22000000));
        radial.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillCircle(w + 16, 16, 16, radial);

        PaintCtx->setCompOp(BL_COMP_OP_CLEAR);
        PaintCtx->setFillStyle(BLRgba32(0xFF000000));
        PaintCtx->fillBox(w, 0, w + 16, 16);
        PaintCtx->fillBox(w + 16, 16, w + 32, 32);
        PaintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    }
    { //右下
        BLGradient radial(BLRadialGradientValues(w + 16, h + 16, w + 16, h + 16, 16));
        radial.addStop(0.0, BLRgba32(0x22000000));
        radial.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillCircle(w + 16, h + 16, 16, radial);

        PaintCtx->setCompOp(BL_COMP_OP_CLEAR);
        PaintCtx->setFillStyle(BLRgba32(0xFF000000));
        PaintCtx->fillBox(w + 16, h, w + 32, h + 16);
        PaintCtx->fillBox(w, h + 16, w + 16, h + 32);
        PaintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    }
    { //左下
        BLGradient radial(BLRadialGradientValues(16, h + 16, 16, h + 16, 16));
        radial.addStop(0.0, BLRgba32(0x22000000));
        radial.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillCircle(16, h + 16, 16, radial);

        PaintCtx->setCompOp(BL_COMP_OP_CLEAR);
        PaintCtx->setFillStyle(BLRgba32(0xFF000000));
        PaintCtx->fillBox(0, h, 16, h + 16);
        PaintCtx->fillBox(16, h + 16, 32, h + 32);
        PaintCtx->setCompOp(BL_COMP_OP_SRC_OVER);
    }
    { //上
        BLGradient linear(BLLinearGradientValues(0, 0, 0, 16));
        linear.addStop(0.0, BLRgba32(0x00000000));
        linear.addStop(1.0, BLRgba32(0x22000000));
        PaintCtx->fillBox(16, 0, w + 16, 16, linear);
    }
    { //右
        BLGradient linear(BLLinearGradientValues(w + 16, 0, w + 32, 0));
        linear.addStop(0.0, BLRgba32(0x22000000));
        linear.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillBox(w + 16, 16, w + 32, h + 16, linear);
    }
    { //下
        BLGradient linear(BLLinearGradientValues(0, h + 16, 0, h + 32));
        linear.addStop(0.0, BLRgba32(0x22000000));
        linear.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillBox(16, h + 16, w + 16, h + 32, linear);
    }
    { //左
        BLGradient linear(BLLinearGradientValues(0, 0, 16, 0));
        linear.addStop(0.0, BLRgba32(0x00000000));
        linear.addStop(1.0, BLRgba32(0x22000000));
        PaintCtx->fillBox(0, 16, 16, h + 16, linear);
    }
}