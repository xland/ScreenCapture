#include "PinWin.h"
#include "History.h"

PinWin::PinWin(const int& x, const int& y, BLImage* img)
{
    state = State::maskReady;    
	this->x = x-16;
	this->y = y-16;
	this->w = img->width() + 32;
	this->h = img->height() + 32;
    OriginalImage = img;
	InitLayerImg();
	InitWindow();
	Show();
}
PinWin::~PinWin()
{

}

void PinWin::BeforePaint() {
    PaintCtx->begin(*BottomImage);
    PaintCtx->clearAll();
    drawShadow();
    PaintCtx->blitImage(BLPoint(16, 16), *OriginalImage);
    PaintCtx->end();
}

int PinWin::OnHitTest(const int& x, const int& y) {
    return HTCAPTION;
}

void PinWin::drawShadow()
{
    unsigned w = this->w - 32;
    unsigned h = this->h - 32;

    {//左上
        BLGradient radial(BLRadialGradientValues(16, 16, 16, 16, 16));
        radial.addStop(0.0, BLRgba32(0x12000000));
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
        radial.addStop(0.0, BLRgba32(0x12000000));
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
        radial.addStop(0.0, BLRgba32(0x12000000));
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
        radial.addStop(0.0, BLRgba32(0x12000000));
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
        linear.addStop(1.0, BLRgba32(0x12000000));
        PaintCtx->fillBox(16, 0, w + 16, 16, linear);
    }
    { //右
        BLGradient linear(BLLinearGradientValues(w + 16, 0, w + 32, 0));
        linear.addStop(0.0, BLRgba32(0x12000000));
        linear.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillBox(w + 16, 16, w + 32, h + 16, linear);
    }
    { //下
        BLGradient linear(BLLinearGradientValues(0, h + 16, 0, h + 32));
        linear.addStop(0.0, BLRgba32(0x12000000));
        linear.addStop(1.0, BLRgba32(0x00000000));
        PaintCtx->fillBox(16, h + 16, w + 16, h + 32, linear);
    }
    { //左
        BLGradient linear(BLLinearGradientValues(0, 0, 16, 0));
        linear.addStop(0.0, BLRgba32(0x00000000));
        linear.addStop(1.0, BLRgba32(0x12000000));
        PaintCtx->fillBox(0, 16, 16, h + 16, linear);
    }
}