#include "pch.h"
#include "App.h"
#include "Win/WinPin.h"
#include "ShapeTextWin.h"
#include "ShapeText.h"
#include "Util.h"
static constexpr int timerID{ 18 };
std::unique_ptr<ShapeTextWin> shapeTextWin;

ShapeTextWin::ShapeTextWin(const int& x, const int& y, const int& w, const int& h) : WinBase(x, y, w, h)
{
}

ShapeTextWin::~ShapeTextWin()
{

}

ShapeTextWin* ShapeTextWin::get()
{
	if (!shapeTextWin.get()) {
		shapeTextWin = std::make_unique<ShapeTextWin>(-99999, -99999, 120, 44);
		shapeTextWin->createWindow(WS_EX_TOPMOST);
	}
	return shapeTextWin.get();
}

void ShapeTextWin::setShape(ShapeText* shape)
{
    this->shape = shape;
    render->CreateSolidColorBrush(shape->color, shape->textBrush.GetAddressOf());
    move((int)shape->winX, (int)shape->winY);
    shape->setTextLayout();
    POINT mousePos{0,0};
    GetCursorPos(&mousePos);
	ScreenToClient(hwnd, &mousePos);
	setCaretByMousePos(mousePos.x, mousePos.y);
    caretVisible = true;
    refresh();
    setTimer(800, timerID);
    show();
}
void ShapeTextWin::changeState()
{
    if (!shape || !shape->isEditing) return;
    shape->setAttr();
    render->CreateSolidColorBrush(shape->color, shape->textBrush.GetAddressOf());
    shape->setTextLayout();
    refresh();
}
void ShapeTextWin::onCreated()
{
    render->CreateSolidColorBrush(D2D1::ColorF(0x99C9EF, 0.6), textSelectionBgBrush.GetAddressOf());
}

void ShapeTextWin::onPaint()
{
    render->Clear(0);
    paintSelectionBg();
    // 绘制文本
    render->DrawTextLayout(D2D1_POINT_2F{ 0, 0 }, shape->textLayout.Get(), shape->textBrush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    if (caretVisible) {
        //绘制输入光标（caretVisible用于控制闪烁）
        render->DrawLine(caretPos, { caretPos.x,caretPos.y + caretHeight }, shape->textBrush.Get(), 1.0f * dpi);
    }
}
void ShapeTextWin::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) // 测试
{
    BOOL isTrailingHit;
    BOOL isInside;
    DWRITE_HIT_TEST_METRICS caretMetrics;
    shape->textLayout->HitTestPoint(x, y, &isTrailingHit, &isInside, &caretMetrics);
    auto textPos = caretMetrics.textPosition;
    if (isTrailingHit) textPos += 1;
    if (caretMetrics.textPosition < caretSelectionDown) { //向后拖拽鼠标
        caretSelectionStart = textPos;
        caretSelectionEnd = caretSelectionDown;
    }
    if (caretMetrics.textPosition > caretSelectionStart) { //向前拖拽鼠标
        caretSelectionEnd = textPos;
        caretSelectionStart = caretSelectionDown;
    }
    resetCaretPos(textPos);
    refresh();
}

BOOL ShapeTextWin::onCursor()
{
    setCursor(IDC_IBEAM);
	return TRUE;
}


void ShapeTextWin::onIME()
{
    if (HIMC himc = ImmGetContext(hwnd))
    {
		//输入法提示框位置，设置在输入光标附近（不能在窗口边缘，不然提示框位置会出现在屏幕右上角）
        POINT pt{ (std::min)(caretPos.x + 1, (float)w - 1), (std::min)((float)h - 1, caretPos.y) };
        COMPOSITIONFORM comp{};
        comp.dwStyle = CFS_FORCE_POSITION;
        comp.ptCurrentPos = pt;
        ImmSetCompositionWindow(himc, &comp);
        CANDIDATEFORM cand{};
        cand.dwStyle = CFS_CANDIDATEPOS;
        cand.ptCurrentPos = pt;
        ImmSetCandidateWindow(himc, &cand);
        ImmReleaseContext(hwnd, himc);
    }
}
void ShapeTextWin::onBlur()
{
	killTimer(timerID);
	caretVisible = false;
    hide();
    shape->isEditing = false;
	shape->win->refresh();
}
void ShapeTextWin::onShown()
{
    SetForegroundWindow(hwnd);
    SetFocus(hwnd);
}

LRESULT ShapeTextWin::onMouseActivate()
{
    return MA_ACTIVATE;
}
void ShapeTextWin::onHidden()
{}
void ShapeTextWin::onChar(const UINT& code)
{
    if (code < 0x20 && code != 9) return;
    delSelection();
    caretSelectionStart = 0;
    caretSelectionEnd = 0;
    UINT32 charsLength = 1;
    wchar_t chars[2] = { static_cast<wchar_t>(code), 0 };
    if (code > 0xFFFF)
    {
        // 处理多字节字符，From http://unicode.org/faq/utf_bom.html#35
        chars[0] = wchar_t(0xD800 + (code >> 10) - (0x10000 >> 10));
        chars[1] = wchar_t(0xDC00 + (code & 0x3FF));
        charsLength++;
    }
    shape->text.insert(caretSelectionDown, chars, charsLength);
    shape->setTextLayout();
    caretSelectionDown += 1;
    resetCaretPos(caretSelectionDown);
    refresh();
}
void ShapeTextWin::onTimer(const UINT& timerId)
{
    if (timerId != timerID) return;
    if (isMouseDown) return;
    caretVisible = !caretVisible;
    refresh();
}
void ShapeTextWin::onMouseDown(const int& x, const int& y, bool isRight)
{
    SetFocus(hwnd);
    BOOL isTrailingHit;
    BOOL isInside;
    DWRITE_HIT_TEST_METRICS caretMetrics;
    shape->textLayout->HitTestPoint(x, y, &isTrailingHit, &isInside, &caretMetrics);
    caretPos.x = caretMetrics.left;
    auto textPos = caretMetrics.textPosition;
    if (isTrailingHit) {
        caretPos.x += caretMetrics.width;
        textPos += 1;
    }
    caretPos.y = caretMetrics.top;
    caretHeight = caretMetrics.height;
    caretSelectionDown = textPos;
    caretSelectionStart = 0;
    caretSelectionEnd = 0;
    refresh();
}
void ShapeTextWin::onMouseUp(const int& x, const int& y)
{
    caretVisible = true;
    refresh();
}
void ShapeTextWin::onKeyDown(const UINT& key)
{
    bool isCtrlDown = (GetKeyState(VK_CONTROL) & 0x8000) != 0;

    if (key == VK_RETURN) {
        delSelection();
        shape->text.insert(caretSelectionDown, 1, L'\n');
        shape->setTextLayout();
        caretSelectionDown += 1;
        resetCaretPos(caretSelectionDown);
        refresh(); //todo 多一次refresh
    }
    else if (key == VK_BACK) {
        if (!delSelection()) {
            if (caretSelectionDown == 0) return;
            shape->text.erase(caretSelectionDown - 1, 1);
            shape->setTextLayout();
            caretSelectionDown -= 1;
            resetCaretPos(caretSelectionDown);
            refresh();
        }
    }
    else if (key == VK_DELETE) {
        if (!delSelection()) {
            if (caretSelectionDown == shape->text.length() - 1) return;
            shape->text.erase(caretSelectionDown, 1);
            shape->setTextLayout();
            resetCaretPos(caretSelectionDown);
            refresh();
        }
    }
    else if (key == VK_TAB) {
        delSelection();
        std::wstring tabStr{ L"    " };
        shape->text.insert(caretSelectionDown, tabStr);
        shape->setTextLayout();
        caretSelectionDown += tabStr.length();
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (key == VK_LEFT) {
        if (caretSelectionDown == 0) return;
        caretSelectionStart = 0;
        caretSelectionEnd = 0;
        caretSelectionDown -= 1;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (key == VK_RIGHT) {
        if (caretSelectionDown == shape->text.length() - 1) return;
        caretSelectionStart = 0;
        caretSelectionEnd = 0;
        caretSelectionDown += 1;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (key == VK_UP) {
        caretSelectionStart = 0;
        caretSelectionEnd = 0;
        BOOL isTrailingHit;
        BOOL isInside;
        DWRITE_HIT_TEST_METRICS newMetrics;
        shape->textLayout->HitTestPoint(caretPos.x, caretPos.y - caretHeight, //上一行的y坐标
            &isTrailingHit, &isInside, &newMetrics);
        caretSelectionDown = newMetrics.textPosition;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (key == VK_DOWN) {
        caretSelectionStart = 0;
        caretSelectionEnd = 0;
        BOOL isTrailingHit;
        BOOL isInside;
        DWRITE_HIT_TEST_METRICS newMetrics;
        shape->textLayout->HitTestPoint(caretPos.x, caretPos.y + caretHeight, //下一行的y坐标
            &isTrailingHit, &isInside, &newMetrics);
        caretSelectionDown = newMetrics.textPosition;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (key == VK_HOME) {
        caretSelectionStart = 0;
        caretSelectionEnd = 0;
        BOOL isTrailingHit;
        BOOL isInside;
        DWRITE_HIT_TEST_METRICS newMetrics;
        shape->textLayout->HitTestPoint(0, //行首
            caretPos.y, &isTrailingHit, &isInside, &newMetrics);
        caretSelectionDown = newMetrics.textPosition;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (key == VK_END) { // end of line
        caretSelectionStart = 0;
        caretSelectionEnd = 0;
        BOOL isTrailingHit;
        BOOL isInside;
        DWRITE_HIT_TEST_METRICS newMetrics;
        shape->textLayout->HitTestPoint(999999, //行尾
            caretPos.y, &isTrailingHit, &isInside, &newMetrics);
        caretSelectionDown = newMetrics.textPosition;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
    else if (isCtrlDown && key == 'C') {
        if (caretSelectionStart == caretSelectionEnd) return;
        std::wstring sub = shape->text.substr(caretSelectionStart, caretSelectionEnd - caretSelectionStart);
        Util::setTextToClipboard(sub);
    }
    else if (isCtrlDown && key == 'V') {
        delSelection();
        auto str = Util::getTextFromClipboard();
        shape->text.insert(caretSelectionDown, str);
        shape->setTextLayout();
        caretSelectionDown += str.size();
        resetCaretPos(caretSelectionDown);
        refresh();
        //auto strOp = Util::getTextFromClipboard2();
        //strOp.Completed([this](auto const& sender, auto status) {
        //    auto str = sender.GetResults();
        //    text.insert(caretSelectionDown, str);
        //    painter->createTextLayout(textLayout.put(), textFormat.get(), text);
        //    caretSelectionDown += str.size();
        //    resetCaretPos(caretSelectionDown);
        //    InvalidateRect(hwnd, nullptr, FALSE);
        //    });
    }
    else if (isCtrlDown && key == 'X') {
        if (caretSelectionStart == caretSelectionEnd) return;
        std::wstring sub = shape->text.substr(caretSelectionStart, caretSelectionEnd - caretSelectionStart);
        Util::setTextToClipboard(sub);
        delSelection();
    }
    else if (isCtrlDown && key == 'A') {
        caretSelectionStart = 0;
        caretSelectionEnd = shape->text.length();
        caretSelectionDown = caretSelectionEnd;
        resetCaretPos(caretSelectionDown);
        refresh();
    }
}

bool ShapeTextWin::delSelection()
{
    if (caretSelectionStart == caretSelectionEnd) return false;
    int len = caretSelectionEnd - caretSelectionStart;
    shape->text.erase(caretSelectionStart, len);
    caretSelectionDown = caretSelectionStart;
    caretSelectionStart = 0;
    caretSelectionEnd = 0;
    shape->setTextLayout();
    resetCaretPos(caretSelectionDown);
    refresh();
    return true;
}
void ShapeTextWin::setCaretByMousePos(const float& x, const float& y)
{
    BOOL isTrailingHit;
    BOOL isInside;
    DWRITE_HIT_TEST_METRICS caretMetrics;
    //给定一个相对于文本布局左上角的像素坐标点，判断该点是否落在文本区域内，并返回与该点对应的文本位置和命中信息。
    shape->textLayout->HitTestPoint(x, y, &isTrailingHit, &isInside, &caretMetrics);
    caretPos.x = caretMetrics.left;
    auto textPos = caretMetrics.textPosition;//命中字符索引
    if (isTrailingHit) { //用户点击的位置更靠近某个字符的“后导边”还是“前导边”
        caretPos.x += caretMetrics.width;
        textPos += 1;
    }
    caretPos.y = caretMetrics.top;
    caretHeight = caretMetrics.height;
    caretSelectionDown = textPos;
    caretSelectionStart = 0;
    caretSelectionEnd = 0;
}
void ShapeTextWin::resetCaretPos(const int& textIndex)
{
    FLOAT xStart, yStart;
    DWRITE_HIT_TEST_METRICS hitStart;
    //从文本位置推算坐标（输入光标所在的文本的位置->输入光标的坐标）
    shape->textLayout->HitTestTextPosition(textIndex, FALSE, &xStart, &yStart, &hitStart);
    caretPos.x = xStart;
    caretPos.y = yStart;
    caretVisible = true;
}
void ShapeTextWin::paintSelectionBg()
{
    if (caretSelectionStart == caretSelectionEnd) return;
    UINT32 lineCount = 0;
    shape->textLayout->GetLineMetrics(nullptr, 0, &lineCount);
    std::vector<DWRITE_LINE_METRICS> lines(lineCount);
    shape->textLayout->GetLineMetrics(lines.data(), lineCount, &lineCount);
    UINT32 lineStart = 0;
    bool isInOneLine = true;
    for (auto& lm : lines) {
        if (lineStart <= caretSelectionStart && lineStart + lm.length > caretSelectionStart) //第一次找到，选择起点落在这一行
        {
            if (caretSelectionEnd <= lineStart + lm.length - 1) { //选择终点也落在这一行
                paintSelectionBgOneRown(caretSelectionStart, caretSelectionEnd);
                break;
            }
            else //选择终点没落在这一行，说明是跨行选择
            {
                paintSelectionBgOneRown(caretSelectionStart, lineStart + lm.length - 1);
                isInOneLine = false;
            }
        }
        else if (!isInOneLine) { //找到选择起点之后的循环
            if (caretSelectionEnd <= lineStart + lm.length - 1) { //选择终点也落在此行
                paintSelectionBgOneRown(0, caretSelectionEnd);
                break;
            }
            else //还没找到选择终点，说明跨多行选择
            {
                paintSelectionBgOneRown(0, lineStart + lm.length - 1);
            }

        }
        lineStart += lm.length;
    }
}
void ShapeTextWin::paintSelectionBgOneRown(const int& start, const int& end)
{
    FLOAT xStart, yStart, xEnd;
    DWRITE_HIT_TEST_METRICS hitStart;
    shape->textLayout->HitTestTextPosition(start, FALSE, &xStart, &yStart, &hitStart);
    shape->textLayout->HitTestTextPosition(end, FALSE, &xEnd, &yStart, &hitStart);
    D2D1_RECT_F rect = D2D1::RectF(xStart, yStart, xEnd, yStart + caretHeight);
    render->FillRectangle(rect, textSelectionBgBrush.Get());
}