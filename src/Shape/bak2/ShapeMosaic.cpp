#include "ShapeMosaic.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "include/effects/Sk2DPathEffect.h"
#include "include/core/SkBlurTypes.h"
#include "include/effects/SkBlurMaskFilter.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkColor.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"
#include "include/core/SkBitmap.h"


ShapeMosaic::ShapeMosaic(const int &x, const int &y) : ShapeBase(x, y)
{    
    initParams();
}

ShapeMosaic::~ShapeMosaic()
{
}

bool ShapeMosaic::OnPaintFinish(SkCanvas *canvas)
{
    auto win = WindowMain::get();
    auto rect = path.getBounds();
    size_t l = ((int)(rect.left() / size)) * size;
    size_t t = ((int)(rect.top() / size)) * size;
    size_t r = ((int)(rect.right() / size) + 1) * size;
    size_t b = ((int)(rect.bottom() / size) + 1) * size;
    rect.setLTRB(l, t, r, b);
    if (rect.width() <= 0 || rect.height() <= 0) {
        return false;
    }
    SkImageInfo info = SkImageInfo::MakeN32Premul(rect.width(), rect.height());
    SkPaint paint;
    SkColor4f colorSum = { 0, 0, 0, 0 };
    int count{ 0 };
    for (size_t x=l; x < r; x+=size)
    {
        for (size_t y = t; y < b; y+=size)
        {
            count = 0;
            for (size_t x1 = 0; x1 < size; x1+=2)
            {
                for (size_t y1 = 0; y1 < size; y1+=2)
                {
                    auto currentColor = win->pixBack->getColor4f(x+x1, y+y1);
                    colorSum.fR += currentColor.fR;
                    colorSum.fG += currentColor.fG;
                    colorSum.fB += currentColor.fB;
                    count++;
                }
            }
            colorSum.fR /= count;
            colorSum.fG /= count;
            colorSum.fB /= count;
            colorSum.fA = 255;
            paint.setColor4f(colorSum);
            canvas->drawRect(SkRect::MakeXYWH(x, y, size, size), paint);
        }
    }
    canvas->saveLayer(nullptr,nullptr);
    unsigned char* addr = new unsigned char[rect.height()*rect.width()*4];
    SkPixmap pixmap(info,addr,rect.width()*4);
    win->pixBack->readPixels(pixmap, rect.left(), rect.top());
    SkBitmap bm;
    bm.installPixels(pixmap);
    canvas->drawImage(bm.asImage(), rect.left(), rect.top());
    paint.setAntiAlias(true);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);
    paint.setBlendMode(SkBlendMode::kClear);
    canvas->drawPath(path, paint);
    canvas->restore();
    delete[] addr;
    return false;
}

bool ShapeMosaic::OnMouseDown(const int &x, const int &y)
{
    path.moveTo(x, y);
    return false;
}

bool ShapeMosaic::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    path.lineTo(x, y);
    WindowMain::get()->Refresh();
    return false;
}

void ShapeMosaic::initParams()
{
    HoverIndex = 4;
    path.moveTo(startX, startY);
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 36;
        }
        else if (stroke == 2)
        {
            strokeWidth = 66;
        }
        else
        {
            strokeWidth = 96;
        }
    }
}