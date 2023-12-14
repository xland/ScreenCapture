#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkRect.h"
#include "include/core/SkPoint.h"
#include "EventHandler.h"
#include <vector>

class CutMask : public EventHandler
{
public:
    ~CutMask();
    static void init();
    static CutMask* get();
    bool OnMouseDown(int x, int y) override;
    bool OnMouseUp(int x, int y) override;
    bool OnMouseMove(int x, int y) override;
    bool OnPaint(SkCanvas *base, SkCanvas *board, SkCanvas *canvas) override;
    bool OnPaintFinish(SkCanvas *base) override;
    SkRect CutRect;

private:
    CutMask();
    std::vector<SkRect> masks;
    SkPoint start;
};