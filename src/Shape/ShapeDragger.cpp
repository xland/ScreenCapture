#include "ShapeDragger.h"
#include "../WindowMain.h"
#include "../Timer.h"
#include "../Recorder.h"

ShapeDragger* shapeDragger;
ShapeDragger::ShapeDragger()
{
    for (size_t i = 0; i < 8; i++)
    {
        draggers.push_back(SkRect::MakeEmpty());
        cursors.push_back(Icon::cursor::all);
    }
}

ShapeDragger::~ShapeDragger()
{
    delete shapeDragger;
}

void ShapeDragger::init()
{
    shapeDragger = new ShapeDragger();
}

ShapeDragger* ShapeDragger::get()
{
    return shapeDragger;
}

void ShapeDragger::setDragger(size_t index, float x, float y)
{
    draggers[index].setXYWH(x, y, size, size);
}

void ShapeDragger::showDragger(const int& shapeIndex)
{
    auto win = WindowMain::get();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : draggers) {
        canvas->drawRect(dragger, paint);
    }
    visible = true;
    this->shapeIndex = shapeIndex;
    win->Refresh();
}

void ShapeDragger::hideDragger()
{
    auto recorder = Recorder::get();
    if (recorder->curIndex != -1) {
        return;
    }
    shapeIndex = -1;
    auto win = WindowMain::get();
    auto canvas = win->surfaceFront->getCanvas();
    canvas->clear(SK_ColorTRANSPARENT);
    win->Refresh();
    visible = false;
}

int ShapeDragger::indexMouseAt(const int& x, const int& y)
{
    if (!visible) {
        return -1;
    }
    for (size_t i = 0; i < draggers.size(); i++)
    {
        if (draggers[i].contains(x, y)) {
            Icon::myCursor(cursors[i]);
            return i;
        }
    }
    return -1;
}


