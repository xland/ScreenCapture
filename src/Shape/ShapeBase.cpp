#include "ShapeBase.h"
#include "../App.h"
#include "../WindowBase.h"
#include "../Recorder.h"

ShapeBase::ShapeBase(const int& x, const int& y):startX{x},startY{y}
{
}

ShapeBase::~ShapeBase()
{
}

bool ShapeBase::MouseInDragger(const int& x, const int& y)
{
    for (size_t i = 0; i < Draggers.size(); i++)
    {
        if (Draggers[i].contains(x, y)) {
            if (HoverIndex > -1) {
                Cursor::SetCursor(DraggerCursors[HoverIndex]); //Dragger已经显示，但Cursor是错误的，这里纠正
            }
            HoverIndex = i;
            return true;
        }
    }
    return false;
}

void ShapeBase::HideDragger()
{
    HoverIndex = -1;
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    win->Refresh();
    Cursor::Cross();
}

void ShapeBase::ShowDragger()
{
    auto win = App::GetWin();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : Draggers) {
        canvas->drawRect(dragger, paint);
    }
    win->Refresh();
    Cursor::SetCursor(DraggerCursors[HoverIndex]);
}
