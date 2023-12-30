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

ShapeMosaic::ShapeMosaic(const int &x, const int &y) : ShapeBase(x, y)
{    
    initParams();
}

ShapeMosaic::~ShapeMosaic()
{
}

void ShapeMosaic::Paint(SkCanvas *canvas)
{
    auto win = WindowMain::get();
    SkPathMeasure pathMeasure(path, false);
    float length = pathMeasure.getLength();
    SkPoint point;
    for (float distance = 0; distance < length; distance += size) {
        if (pathMeasure.getPosTan(distance, &point, nullptr)) {
            drawRectsByPoints(point, canvas);
        }
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStroke(true);
    paint.setStrokeWidth(strokeWidth);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeCap(SkPaint::Cap::kRound_Cap);
    paint.setStrokeJoin(SkPaint::kRound_Join);  
    paint.setBlendMode(SkBlendMode::kClear);
    path.setFillType(SkPathFillType::kInverseWinding);
    canvas->drawPath(path, paint);
}

bool ShapeMosaic::OnMouseDown(const int &x, const int &y)
{
    auto win = WindowMain::get();
    auto surfaceFront = win->surfaceFront;
    surfaceFront->writePixels(*win->pixSrc, 0, 0);
    auto canvas = win->surfaceFront->getCanvas();
    canvas->drawImage(win->surfaceBack->makeImageSnapshot(), 0, 0);
    SkImageInfo info = SkImageInfo::MakeN32Premul(win->w, win->h);
    auto addr = new unsigned char[win->w * win->h * 4];
    pixmap = new SkPixmap(info, addr, win->w * 4);
    surfaceFront->readPixels(*pixmap,0,0);
    canvas->clear(SK_ColorTRANSPARENT);
    path.moveTo(x, y);
    return false;
}

bool ShapeMosaic::OnMouseUp(const int &x, const int &y)
{
    //auto win = WindowMain::get();
    //auto canvas = win->surfaceBack->getCanvas();
    //canvas->clear(SK_ColorTRANSPARENT);
    //Paint(canvas);
    //win->Refresh();
    delete[] pixmap->addr();
    delete pixmap;
    pixmap = nullptr;
    colorCache.clear();
    return false;
}

bool ShapeMosaic::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeMosaic::OnMoseDrag(const int &x, const int &y)
{
    isWip = false;
    path.lineTo(x, y);
    auto win = WindowMain::get();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    win->Refresh();
    return false;
}

void ShapeMosaic::drawRectsByPoints(const SkPoint& point, SkCanvas* canvas)
{
    auto win = WindowMain::get();
    int rowNum = std::ceil(win->w / size);
    int rectNum = std::ceil(strokeWidth*2 / size);
    int xIndex = (point.fX - strokeWidth) / size;
    int yIndex = (point.fY - strokeWidth) / size;
    SkColor4f colorSum = { 0, 0, 0, 0 };
    SkPaint paint;
    for (size_t i = yIndex; i < yIndex+rectNum; i++)
    {
        for (size_t j = xIndex; j < xIndex+rectNum; j++)
        {
            auto key = i*rowNum+j;
            auto x = j * size;
            auto y = i * size;
            if (colorCache.contains(key)) {
                paint.setColor(colorCache[key]);
                canvas->drawRect(SkRect::MakeXYWH(x, y, size, size),paint);
            }
            else {
                int count{ 0 };
                for (size_t x1 = x; x1 <= x+size; x1 += 2)
                {
                    for (size_t y1 = y; y1 <= y+size; y1 += 2)
                    {
                        auto currentColor = pixmap->getColor4f(x1, y1);
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
                auto color = colorSum.toSkColor();
                paint.setColor(color);
                canvas->drawRect(SkRect::MakeXYWH(x, y, size, size), paint);
                colorCache.insert({ key, color });
            }
        }
    }

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