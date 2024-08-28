#include "CutMask.h"
#include "App.h"
#include "WinMax.h"
#include <dwmapi.h>
#include <include/core/SkPath.h>
#include <include/core/SkFont.h>
#include "Font.h"
#include <format>
#include <string>
#include "Lang.h"
#include "TypeDefine.h"


CutMask::CutMask()
{
}

CutMask::~CutMask()
{
}

void CutMask::Init()
{
    enumWinRects();
    listenMouseMove(std::bind(&CutMask::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2));
    listenMouseDrag(std::bind(&CutMask::OnMouseDrag, this, std::placeholders::_1, std::placeholders::_2));
    listenLeftBtnDown(std::bind(&CutMask::OnLeftBtnDown, this, std::placeholders::_1, std::placeholders::_2));
    listenLeftBtnUp(std::bind(&CutMask::OnLeftBtnUp, this, std::placeholders::_1, std::placeholders::_2));
    listenPaint(std::bind(&CutMask::OnPaint, this, std::placeholders::_1));
}

void CutMask::OnPaint(SkCanvas* canvas)
{
    if (cutRect.isEmpty()) {
        return;
    }
    paintRect(canvas);
    paintInfo(canvas);
}
void CutMask::paintRect(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(0xAA000000);
    SkPath path;
    path.addRect(cutRect);
    path.setFillType(SkPathFillType::kInverseEvenOdd);
    canvas->drawPath(path, paint);
    paint.setColor(0XFF4096ff);
    paint.setStrokeWidth(2.8);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    canvas->drawRect(cutRect, paint);
}
void CutMask::paintInfo(SkCanvas* canvas)
{
    auto font = Font::GetText();
    auto str = std::format(L"{}:{} {}:{} {}:{} {}:{} {}:{} {}:{}",
        Lang::Get(Lang::Key::Left), cutRect.fLeft,
        Lang::Get(Lang::Key::Top), cutRect.fTop,
        Lang::Get(Lang::Key::Right), cutRect.fRight,
        Lang::Get(Lang::Key::Bottom), cutRect.fBottom,
        Lang::Get(Lang::Key::Width), cutRect.width(),
        Lang::Get(Lang::Key::Height), cutRect.height());
    auto win = App::GetWin();
    font->setSize(12*win->scaleFactor);
    auto data = str.data();
    auto len = str.size() * 2;
    SkRect rectTemp;
    font->measureText(data, len, SkTextEncoding::kUTF16, &rectTemp);

    SkRect rectInfo = SkRect::MakeXYWH(cutRect.fLeft, (cutRect.fTop - rectTemp.height() - 18), 
        (rectTemp.width() + 16), rectTemp.height()+10);
    if (cutRect.fTop < rectInfo.height()+6) {
        rectInfo.offset(6, rectInfo.height()+15);
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStroke(false);
    paint.setColor(0x88000000);
    canvas->drawRoundRect(rectInfo, 3, 3, paint);
    paint.setColor(0xFFFFFFFF);
    auto x = rectInfo.centerX() - rectTemp.width() / 2 - rectTemp.fLeft;
    auto y = rectInfo.centerY() - rectTemp.height() / 2 - rectTemp.fTop;
    canvas->drawSimpleText(data, len, SkTextEncoding::kUTF16, x, y+1, *font, paint);
}
void CutMask::OnLeftBtnDown(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state == State::start) {
        cutRect.setLTRB(x, y, x, y);
        hoverIndex = 4;
        win->UpdateState(State::mask);
        return;
    }
    if (hoverIndex == 0) {
        cutRect.setLTRB(x, y, cutRect.fRight, cutRect.fBottom);
        start.set(cutRect.fRight, cutRect.fBottom);
    }
    else if (hoverIndex == 1) {
        cutRect.setLTRB(cutRect.fLeft, y, cutRect.fRight, cutRect.fBottom);
        start.set(0, cutRect.fBottom);
    }
    else if (hoverIndex == 2) {
        cutRect.setLTRB(cutRect.fLeft, y, x, cutRect.fBottom);
        start.set(cutRect.fLeft, cutRect.fBottom);
    }
    else if (hoverIndex == 3) {
        cutRect.setLTRB(cutRect.fLeft, cutRect.fTop, x, cutRect.fBottom);
        start.set(cutRect.fLeft, 0);
    }
    else if (hoverIndex == 4) {
        cutRect.setLTRB(cutRect.fLeft, cutRect.fTop, x, y);
        start.set(cutRect.fLeft, cutRect.fTop);
    }
    else if (hoverIndex == 5) {
        cutRect.setLTRB(cutRect.fLeft, cutRect.fTop, cutRect.fRight, y);
        start.set(0, cutRect.fTop);
    }
    else if (hoverIndex == 6) {
        cutRect.setLTRB(x, cutRect.fTop, cutRect.fRight, y);
        start.set(cutRect.fRight, cutRect.fTop);
    }
    else if (hoverIndex == 7) {
        cutRect.setLTRB(x, cutRect.fTop, cutRect.fRight, cutRect.fBottom);
        start.set(cutRect.fRight, 0);
    }
    else if (hoverIndex == 8) {
        start.set(x - cutRect.fLeft, y - cutRect.fTop);
        return;
    }
    else {
        return;
    }
    win->UpdateState(State::mask);
    win->Refresh();
    return;
}
void CutMask::OnLeftBtnUp(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state != State::mask) {
        return;
    }
    win->UpdateState(State::tool);
    win->Emit(EventType::maskReady);
    return;
}
void CutMask::OnMouseMove(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state < State::mask)
    {
        highLightWinRect(x, y);
        return;
    }
    if (win->state == State::tool) {
        hoverMask(x, y);
        return;
    }
    else {
        hoverBorder(x, y);
        if (hoverIndex != -1) {
            return;
        }
    }
    return;
}
void CutMask::OnMouseDrag(const int& x, const int& y)
{
    if (hoverIndex < 0) {
        return;
    }
    if (start.fX == -10 && start.fY == -10) {
        start.fX = cutRect.fLeft;
        start.fY = cutRect.fTop;
    }
    auto win = App::GetWin();
    win->state = State::mask;
    if (hoverIndex == 0) {
        cutRect.setLTRB(x, y, start.fX, start.fY);
    }
    else if (hoverIndex == 1) {
        cutRect.setLTRB(cutRect.fLeft, y, cutRect.fRight, start.y());
    }
    else if (hoverIndex == 2) {
        cutRect.setLTRB(start.fX, y, x, start.fY);
    }
    else if (hoverIndex == 3) {
        cutRect.setLTRB(start.fX, cutRect.fTop, x, cutRect.fBottom);
    }
    else if (hoverIndex == 4) {
        cutRect.setLTRB(start.fX, start.fY, x, y);
    }
    else if (hoverIndex == 5) {
        cutRect.setLTRB(cutRect.fLeft, start.fY, cutRect.fRight, y);
    }
    else if (hoverIndex == 6) {
        cutRect.setLTRB(x, start.fY, start.fX, y);
    }
    else if (hoverIndex == 7) {
        cutRect.setLTRB(x, cutRect.fTop, start.fX, cutRect.fBottom);
    }
    else if (hoverIndex == 8) {
        auto left = x - start.fX;
        auto top = y - start.fY;
        if (left < 0) left = 0;
        if (top < 0) top = 0;
        auto right = left + cutRect.width();
        auto bottom = top + cutRect.height();
        if (right > win->w) {
            right = win->w;
            left = win->w - cutRect.width();
        }
        if (bottom > win->h) {
            bottom = win->h;
            top = win->h - cutRect.height();
        }
        cutRect.setLTRB(left, top, right, bottom);
        start.set(x - cutRect.fLeft, y - cutRect.fTop);
        win->Refresh();
        return;
    }
    cutRect.sort();
    if (!(cutRect.width() <= 1 && cutRect.height() <= 1)) {
        win->Refresh();
    }
    return;
}
void CutMask::highLightWinRect(const int& x, const int& y)
{
    for (size_t i = 0; i < winRects.size(); i++)
    {
        if (winRects[i].contains(x, y)) {
            if (cutRect.left() != winRects[i].left() ||
                cutRect.top() != winRects[i].top() ||
                cutRect.right() != winRects[i].right() ||
                cutRect.bottom() != winRects[i].bottom()) {
                cutRect = winRects[i];
                auto win = App::GetWin();
                win->Refresh();
            }
            break;
        }
    }
}
void CutMask::hoverMask(const int& x, const int& y)
{
    if (x < cutRect.fLeft+5 && y < cutRect.fTop+5) {
        hoverIndex = 0;
        App::Cursor(IDC_SIZENWSE);
    }
    else if (x > cutRect.fLeft+5 && x < cutRect.fRight-5 && y <= cutRect.fTop + 5) {
        hoverIndex = 1;
        App::Cursor(IDC_SIZENS);
    }
    else if (x > cutRect.fRight-5 && y < cutRect.fTop+5) {
        hoverIndex = 2;
        App::Cursor(IDC_SIZENESW);
    }
    else if (x >= cutRect.fRight - 5 && y > cutRect.fTop+5 && y < cutRect.fBottom-5) {
        hoverIndex = 3;
        App::Cursor(IDC_SIZEWE);
    }
    else if (x > cutRect.fRight-5 && y > cutRect.fBottom-5) {
        hoverIndex = 4;
        App::Cursor(IDC_SIZENWSE);
    }
    else if (x > cutRect.fLeft+5 && x < cutRect.fRight-5 && y >= cutRect.fBottom - 5) {
        hoverIndex = 5;
        App::Cursor(IDC_SIZENS);
    }
    else if (x < cutRect.fLeft+5 && y > cutRect.fBottom-5) {
        hoverIndex = 6;
        App::Cursor(IDC_SIZENESW);
    }
    else if (x <= cutRect.fLeft + 5 && y < cutRect.fBottom-5 && y > cutRect.fTop+5) {
        hoverIndex = 7;
        App::Cursor(IDC_SIZEWE);
    }
    else {
        hoverIndex = 8;
        App::Cursor(IDC_SIZEALL);
    }
}
void CutMask::hoverBorder(const int& x, const int& y)
{
    if (x > cutRect.fLeft - 5 && x < cutRect.fLeft + 5 && y < cutRect.fTop + 5 && y > cutRect.fTop - 5) {
        hoverIndex = 0;
        App::Cursor(IDC_SIZENWSE);
    }
    else if (x > cutRect.fLeft + 5 && x < cutRect.fRight - 5 && y < cutRect.fTop + 5 && y > cutRect.fTop - 5) {
        hoverIndex = 1;
        App::Cursor(IDC_SIZENS);
    }
    else if (x > cutRect.fRight - 5 && x < cutRect.fRight + 5 && y < cutRect.fTop + 5 && y > cutRect.fTop - 5) {
        hoverIndex = 2;
        App::Cursor(IDC_SIZENESW);
    }
    else if (x > cutRect.fRight - 5 && x < cutRect.fRight + 5 && y > cutRect.fTop + 5 && y < cutRect.fBottom - 5) {
        hoverIndex = 3;
        App::Cursor(IDC_SIZEWE);
    }
    else if (x > cutRect.fRight - 5 && x < cutRect.fRight + 5 && y > cutRect.fBottom - 5 && y < cutRect.fBottom + 5) {
        hoverIndex = 4;
        App::Cursor(IDC_SIZENWSE);
    }
    else if (x > cutRect.fLeft + 5 && x < cutRect.fRight - 5 && y < cutRect.fBottom + 5 && y > cutRect.fBottom - 5) {
        hoverIndex = 5;
        App::Cursor(IDC_SIZENS);
    }
    else if (x > cutRect.fLeft - 5 && x < cutRect.fLeft + 5 && y < cutRect.fBottom + 5 && y > cutRect.fBottom - 5) {
        hoverIndex = 6;
        App::Cursor(IDC_SIZENESW);
    }
    else if (x > cutRect.fLeft - 5 && x < cutRect.fLeft + 5 && y > cutRect.fTop + 5 && y < cutRect.fBottom - 5) {
        hoverIndex = 7;
        App::Cursor(IDC_SIZEWE);
    }
    else {
        hoverIndex = -1;
    }
}
void CutMask::enumWinRects()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            int length = GetWindowTextLength(hwnd);
            if (length == 0) {
                return TRUE;
            }
            std::wstring windowTitle(length, '\0');
            GetWindowText(hwnd, &windowTitle[0], length + 1);
            auto self = (CutMask*)lparam;
            auto win = App::GetWin();
            if (rect.left < win->x) rect.left = win->x;
            if (rect.right > win->x + win->w) rect.right = win->x + win->w;
            if (rect.top < win->y) rect.top = win->y;
            if (rect.bottom > win->y + win->h) rect.bottom = win->y + win->h;
            self->winRects.push_back(SkRect::MakeLTRB(rect.left - win->x, rect.top - win->y, rect.right - win->x, rect.bottom - win->y));
            return TRUE;
        }, (LPARAM)this);
}
