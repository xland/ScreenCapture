
#include "include/core/SkCanvas.h"
class EventHandler
{
public:
    EventHandler();
    ~EventHandler();
    virtual bool OnMouseDown(int x, int y) { return false; };
    virtual bool OnMouseUp(int x, int y) { return false; };
    virtual bool OnMouseMove(int x, int y) { return false; };
    virtual bool OnDrag(int x, int y) { return false; };
    virtual bool OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas) { return false; };
    virtual bool OnPaintFinish(SkCanvas *base) { return false; };

protected:
private:
};