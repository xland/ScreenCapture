#include "ShapeText.h"
#include "../WindowMain.h"
#include "../ToolSub.h"

static float fontSize{ 80 };

ShapeText::ShapeText(const int &x, const int &y) : ShapeBase(x, y)
{
    initParams();
}

ShapeText::~ShapeText()
{
}

bool ShapeText::OnMouseDown(const int &x, const int &y)
{
    IsWIP = false;
    rect.setXYWH(x - 15, y - 50, 30, 100);
    activeKeyboard(startX, startY + 25);
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeText::OnMouseMove(const int &x, const int &y)
{
    return false;
}

bool ShapeText::OnMouseUp(const int &x, const int &y)
{
    return false;
}

bool ShapeText::OnMoseDrag(const int &x, const int &y)
{
    IsWIP = false;
    WindowMain::get()->Refresh();
    return false;
}

bool ShapeText::OnPaint(SkCanvas *canvas)
{
    SkPaint paint;
    paint.setColor(color);
    paint.setStroke(true);
    paint.setStrokeWidth(2);
    canvas->drawRect(rect, paint);
    
    //SkRect textBounds;
    //font->measureText(str.c_str(), str.size(), SkTextEncoding::kUTF8, &textBounds);
    //SkScalar x = startX - textBounds.width() / 2 - textBounds.left();
    //SkScalar y = startY + textBounds.height() / 2 - textBounds.bottom();
    //canvas->drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, x, y, *font, paint);


    canvas->drawLine(startX, startY - 25, startX, startY + 25,paint);
    return false;
}

void ShapeText::initParams()
{
    auto tool = ToolSub::get();
    stroke = !tool->getFill();
    if (stroke)
    {
        auto stroke = tool->getStroke();
        if (stroke == 1)
        {
            strokeWidth = 4;
        }
        else if (stroke == 2)
        {
            strokeWidth = 16;
        }
        else
        {
            strokeWidth = 28;
        }
    }
    color = tool->getColor();
}
void ShapeText::activeKeyboard(long x, long y)
{
    auto win = WindowMain::get();
    if (HIMC himc = ImmGetContext(win->hwnd))
    {
        COMPOSITIONFORM comp = {};
        comp.ptCurrentPos.x = x;
        comp.ptCurrentPos.y = y;
        comp.dwStyle = CFS_FORCE_POSITION;
        ImmSetCompositionWindow(himc, &comp);
        CANDIDATEFORM cand = {};
        cand.dwStyle = CFS_CANDIDATEPOS;
        cand.ptCurrentPos.x = x;
        cand.ptCurrentPos.y = y;
        ImmSetCandidateWindow(himc, &cand);
        ImmReleaseContext(win->hwnd, himc);
    }
}
void ShapeText::InsertWord(const std::wstring& word)
{
    text = text.substr(0, cursorIndex) + word + text.substr(cursorIndex);
    cursorIndex += 1;
    WindowMain::get()->Refresh();
}