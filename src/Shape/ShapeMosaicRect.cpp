#include "ShapeMosaicRect.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../ToolSub.h"
#include "../Cursor.h"
#include "../ColorBlender.h"

ShapeMosaicRect::ShapeMosaicRect(const int &x, const int &y) : ShapeRect(x, y)
{
}

ShapeMosaicRect::~ShapeMosaicRect()
{
}

bool ShapeMosaicRect::OnMouseDown(const int& x, const int& y)
{
    startX = x;
    startY = y;
    return false;
}

bool ShapeMosaicRect::OnMouseUp(const int& x, const int& y)
{
    IsWip = false;
    setDragger();
    return false;
}

void ShapeMosaicRect::OnShowDragger(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    canvas->drawRect(rect, paint);
}

void ShapeMosaicRect::drawRectsByPoints(SkCanvas* canvas)
{
    auto win = App::GetWin();
    int columnNum = std::ceil((float)win->w / size); //整个屏幕马赛克的列数
    int rectColumnNum = std::ceil(rect.width() / size) + 1; //绘制区马赛克列数
    int rectRowNum = std::ceil(rect.height() / size) + 1; //绘制区马赛克行数
    int xIndex = rect.fLeft / size; //绘制区左侧第一个马赛克在屏幕中属于第几列
    int yIndex = rect.fTop / size; //绘制区顶部第一个马赛克在屏幕中属于第几行
    xIndex = xIndex < 0 ? 0 : xIndex;
    yIndex = yIndex < 0 ? 0 : yIndex;
    SkColor4f colorSum = { 0, 0, 0, 0 };
    SkPaint paint;
    for (size_t i = yIndex; i < yIndex + rectRowNum; i++)
    {
        for (size_t j = xIndex; j < xIndex + rectColumnNum; j++)
        {
            int key = i * columnNum + j;
            auto x = j * size;
            auto y = i * size;
            int count{ 0 };
            for (size_t x1 = x; x1 <= x + size; x1 += 2)
            {
                for (size_t y1 = y; y1 <= y + size; y1 += 2)
                {
                    SkColor4f currentColor;
                    if (x1 >= win->w || y1 >= win->h) {
                        currentColor = SkColor4f::FromColor(SK_ColorBLACK);
                    }
                    else {
                        auto colorSrc = win->pixSrc->getColor(x1, y1);
                        auto colorBack = win->pixBack->getColor(x1, y1);
                        currentColor = ColorBlender::Blend(colorSrc, colorBack);
                    }
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

void ShapeMosaicRect::Paint(SkCanvas *canvas)
{
    canvas->saveLayer(nullptr, nullptr);
    SkPaint paint;
    drawRectsByPoints(canvas);
    paint.setAntiAlias(true);
    paint.setStroke(false);
    paint.setBlendMode(SkBlendMode::kClear);
    SkPath path;
    path.addRect(rect);
    path.setFillType(SkPathFillType::kInverseWinding);
    canvas->drawPath(path, paint);
    canvas->restore();
}

bool ShapeMosaicRect::OnMouseMove(const int& x, const int& y)
{
    if (MouseInDragger(x, y)) {
        return true;
    }
    if (rect.contains(x, y)) {
        HoverIndex = 8;
        return true;
    }
    return false;
}
