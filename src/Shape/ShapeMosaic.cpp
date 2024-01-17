#include "ShapeMosaic.h"
#include <memory>
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "include/core/SkPathMeasure.h"
#include "include/core/SkColor.h"

ShapeMosaic::ShapeMosaic(const int &x, const int &y) : ShapeBase(x, y)
{
    path.moveTo(startX, startY);
    IsWip = false;
    initParams();
}

ShapeMosaic::~ShapeMosaic()
{
}

void ShapeMosaic::Paint(SkCanvas *canvas)
{
    SkPaint paint;
    SkPathMeasure pathMeasure(path, false);
    float length = pathMeasure.getLength();
    SkPoint point;
    for (float distance = 0; distance < length; distance += size) {
        if (pathMeasure.getPosTan(distance, &point, nullptr)) {
            drawRectsByPoints(point, canvas);
        }
    }
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
    path.moveTo(x, y);
    temp = new SkBitmap();
    temp->allocN32Pixels(1, 1); //todo
    return false;
}

bool ShapeMosaic::OnMouseUp(const int &x, const int &y)
{
    IsWip = false;
    colorCache.clear();
    return false;
}

bool ShapeMosaic::OnMoseDrag(const int &x, const int &y)
{
    IsTemp = false;
    path.lineTo(x, y);
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    Paint(canvas);
    win->Refresh();
    return false;
}

void ShapeMosaic::drawRectsByPoints(const SkPoint& point, SkCanvas* canvas)
{
    SkCanvas colorCanvas(*temp);
    auto win = App::GetWin();
    int rowNum = std::ceil((float)win->w / size);
    int rectNum = std::ceil(strokeWidth*2 / size);
    int xIndex = (point.fX - strokeWidth) / size;
    int yIndex = (point.fY - strokeWidth) / size;
    SkColor4f colorSum = { 0, 0, 0, 0 };
    SkPaint paint;
    for (size_t i = yIndex; i < yIndex+rectNum; i++)
    {
        for (size_t j = xIndex; j < xIndex+rectNum; j++)
        {
            int key = i*rowNum+j;
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
                        auto colorSrc = win->pixSrc->getColor(x1, y1);
                        auto colorBack = win->pixBack->getColor(x1, y1);
                        colorCanvas.drawColor(colorSrc);
                        colorCanvas.drawColor(colorBack);
                        void* pixels = temp->getPixels();
                        auto colorTarget = *(SkColor*)pixels;
                        auto currentColor = SkColor4f::FromColor(colorTarget);
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
    auto tool = ToolSub::Get();
    auto stroke = tool->GetStroke();
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