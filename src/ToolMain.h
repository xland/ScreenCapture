#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "EventHandler.h"
#include <vector>

class ToolMain : public EventHandler
{
public:
    ~ToolMain();
    static void init();
    static ToolMain *get();
    bool OnMouseDown(int x, int y) override;
    bool OnMouseUp(int x, int y) override;
    bool OnMouseMove(int x, int y) override;
    bool OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas) override;
    bool OnPaintFinish(SkCanvas *base) override;

private:
    ToolMain();
};