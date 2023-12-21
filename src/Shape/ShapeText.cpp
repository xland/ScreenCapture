#include "ShapeText.h"
#include "../WindowMain.h"
#include "../ToolSub.h"
#include "../AppFont.h"
#include <locale>
#include <codecvt>
#include "include/core/SkCanvas.h"
#include "include/core/SkPaint.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkString.h"
#include "include/utils/SkTextUtils.h"
#include "include/core/SkFontMetrics.h"


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
    auto font = AppFont::Get()->fontText;
    font->setSize(fontSize);
    auto data = text.data();
    auto length = wcslen(data) * 2;
    SkRect textRect;
    font->measureText(text.data(), length, SkTextEncoding::kUTF16, &textRect);
    //SkFontMetrics metrics;
    //font->getMetrics(&metrics);
    //auto height = metrics.fBottom - metrics.fTop;
    rect.setXYWH(x, y - textRect.height()/2, textRect.width(), textRect.height());
    rect.fLeft -= 10;
    rect.fRight += 10;
    //activeKeyboard(startX, startY + 25);
    //WindowMain::get()->Refresh();
    PostMessage(WindowMain::get()->hwnd, WM_REFRESH, NULL, NULL);
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
    canvas->drawLine(startX, startY - 25, startX, startY + 25, paint);
    SkRect textBounds;
    auto font = AppFont::Get()->fontText;
    std::string str{ "Line 1\nLine 2\nLine 3" };
    //font->measureText(str.c_str(), str.size(), SkTextEncoding::kUTF8, &textBounds);
    //SkScalar x = startX - textBounds.width() / 2 - textBounds.left();
    //SkScalar y = startY + textBounds.height() / 2 - textBounds.bottom();    
    //canvas->drawSimpleText(str.c_str(), str.size(), SkTextEncoding::kUTF8, x, y, *font, paint);


    //paint.setStroke(false);
    //auto data = text.data();
    //auto length = wcslen(data) * 2;
    //auto font = AppFont::Get()->fontText;
    //font->setSize(fontSize);


    //std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    //// 创建多行宽字符文本
    //std::wstring text = L"This is a multi-line text.\nSecond line.";
    //// 分割文本为多行
    //std::vector<SkString> lines;
    //std::wstring::size_type pos = 0;
    //while ((pos = text.find(L'\n', pos)) != std::wstring::npos) {
    //    auto str = converter.to_bytes(text.substr(0, pos));
    //    lines.push_back(SkString(str.c_str()));
    //    text.erase(0, pos + 1);
    //}
    //auto str = converter.to_bytes(text.substr(0, pos));
    //lines.push_back(SkString(str.c_str()));
    //SkFontMetrics metrics;
    //font->getMetrics(&metrics);
    //auto height = metrics.fBottom - metrics.fTop;
    //for (const auto& line : lines) {
    //    SkTextUtils::DrawString(canvas, line.c_str(), startX, startY, *font, paint,SkTextUtils::kLeft_Align);
    //    startY += height; // 下一行的位置在当前行下方加上文本大小
    //}
 


    paint.setColor(SK_ColorRED);
    paint.setStroke(false);
    
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