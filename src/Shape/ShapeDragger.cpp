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

void ShapeDragger::showDragger()
{
    auto win = WindowMain::get();
    auto canvas = win->surfaceFront->getCanvas();
    SkPaint paint;
    paint.setStroke(true);
    paint.setStrokeWidth(1);
    paint.setColor(SK_ColorBLACK);
    for (auto& dragger : draggers) {
        canvas->drawRect(dragger, paint);
    }
    visible = true;
}

void ShapeDragger::hideDragger()
{
    auto recorder = Recorder::get();
    if (recorder->curIndex != -1) {
        return;
    }
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
            return i;
        }
    }
    return -1;
}


