#include "CutMask.h"
#include "WinMax.h"
#include <include/core/SkPath.h>
#include <include/core/SkFont.h>
#include "Font.h"
#include <format>
#include <string>
#include "Lang.h"


CutMask::CutMask()
{
}

CutMask::~CutMask()
{
}

void CutMask::Paint(SkCanvas* canvas)
{
    if (cutRect.isEmpty()) {
        return;
    }
    PaintRect(canvas);
    PaintInfo(canvas);
}

void CutMask::PaintRect(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setColor(0xAA000000);
    SkPath path;
    path.addRect(cutRect);
    path.setFillType(SkPathFillType::kInverseEvenOdd);
    canvas->drawPath(path, paint);
    paint.setColor(0XFF1677ff);
    paint.setStrokeWidth(2.8);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    canvas->drawRect(cutRect, paint);
}

void CutMask::PaintInfo(SkCanvas* canvas)
{
    auto font = Font::Get()->text.get();
    auto str = std::format(L"{}:{}  {}:{}  {}:{}  {}:{}  {}:{}  {}:{}",
        Lang::Get(Lang::Key::Left), cutRect.fLeft,
        Lang::Get(Lang::Key::Top), cutRect.fTop,
        Lang::Get(Lang::Key::Right), cutRect.fRight,
        Lang::Get(Lang::Key::Bottom), cutRect.fBottom,
        Lang::Get(Lang::Key::Width), cutRect.width(),
        Lang::Get(Lang::Key::Height), cutRect.height());
    auto win = WinMax::Get();
    font->setSize(14);
    auto data = str.data();
    auto len = str.size() * 2;
    SkRect rectTemp;
    font->measureText(data, len, SkTextEncoding::kUTF16, &rectTemp);
    SkRect rectInfo = SkRect::MakeXYWH(cutRect.fLeft, cutRect.fTop - 36, (rectTemp.width() + 16), 32);
    if (cutRect.fTop < 38) {
        rectInfo.offset(6, 42);
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setStroke(false);
    paint.setColor(SkColorSetARGB(130, 0, 0, 0));
    canvas->drawRoundRect(rectInfo, 3, 3, paint);
    paint.setColor(SkColorSetARGB(255, 220, 220, 220));
    canvas->drawSimpleText(data, len, SkTextEncoding::kUTF16, rectInfo.fLeft + 8, rectInfo.fTop + 21, *font, paint);
}

bool CutMask::onLeftBtnDown(const int& x, const int& y)
{
    auto win = WinMax::Get();
    if (win->state == State::start) {
        cutRect.setLTRB(x, y, x, y);
        hoverIndex = 4;
        win->state = State::mask;
        win->canvas->clear(SK_ColorTRANSPARENT);
        return true;
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
        return true;
    }
    else {
        return false;
    }
    win->state = State::mask;
    win->refresh();
    return true;
}

bool CutMask::onLeftBtnUp(const int& x, const int& y)
{
    auto win = WinMax::Get();
    if (win->state != State::mask) {
        return false;
    }
    if ((start.fX == -10 && start.fY == -10) || (cutRect.width() <= 1 && cutRect.height() <= 1)) {
        for (size_t i = 0; i < winRects.size(); i++)
        {
            if (winRects[i].contains(x, y)) {
                cutRect = winRects[i];
                break;
            }
        }
    }
    win->state = State::tool;
    return true;
}

bool CutMask::onMouseMove(const int& x, const int& y)
{
    auto win = WinMax::Get();
    if (win->state < State::mask)
    {
        highLightWinRect(x, y);
        return true;
    }
    if (win->state == State::tool) {
        hoverMask(x, y);
        return true;
    }
    else {
        hoverBorder(x, y);
        if (hoverIndex != -1) {
            return true;
        }
    }
    return false;
}

bool CutMask::onMouseDrag(const int& x, const int& y)
{
    if (hoverIndex < 0) {
        return false;
    }
    if (start.fX == -10 && start.fY == -10) {
        start.fX = cutRect.fLeft;
        start.fY = cutRect.fTop;
    }
    auto win = WinMax::Get();
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
        win->refresh();
        return true;
    }
    cutRect.sort();
    if (!(cutRect.width() <= 1 && cutRect.height() <= 1)) {
        win->refresh();
    }
    return true;
}

void CutMask::highLightWinRect(const int& x, const int& y)
{
    for (size_t i = 0; i < winRects.size(); i++)
    {
        if (winRects[i].contains(x, y)) {
            cutRect = winRects[i];
            return;
        }
    }
}

void CutMask::hoverMask(const int& x, const int& y)
{
    if (x < cutRect.fLeft+5 && y < cutRect.fTop+5) {
        hoverIndex = 0;
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
    }
    else if (x > cutRect.fLeft+5 && x < cutRect.fRight-5 && y <= cutRect.fTop + 5) {
        hoverIndex = 1;
        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
    }
    else if (x > cutRect.fRight-5 && y < cutRect.fTop+5) {
        hoverIndex = 2;
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
    }
    else if (x >= cutRect.fRight - 5 && y > cutRect.fTop+5 && y < cutRect.fBottom-5) {
        hoverIndex = 3;
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
    }
    else if (x > cutRect.fRight-5 && y > cutRect.fBottom-5) {
        hoverIndex = 4;
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
    }
    else if (x > cutRect.fLeft+5 && x < cutRect.fRight-5 && y >= cutRect.fBottom - 5) {
        hoverIndex = 5;
        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
    }
    else if (x < cutRect.fLeft+5 && y > cutRect.fBottom-5) {
        hoverIndex = 6;
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
    }
    else if (x <= cutRect.fLeft + 5 && y < cutRect.fBottom-5 && y > cutRect.fTop+5) {
        hoverIndex = 7;
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
    }
    else {
        hoverIndex = 8;
        SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
    }
}

void CutMask::hoverBorder(const int& x, const int& y)
{
    if (x > cutRect.fLeft - 5 && x < cutRect.fLeft + 5 && y < cutRect.fTop + 5 && y > cutRect.fTop - 5) {
        hoverIndex = 0;
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
    }
    else if (x > cutRect.fLeft + 5 && x < cutRect.fRight - 5 && y < cutRect.fTop + 5 && y > cutRect.fTop - 5) {
        hoverIndex = 1;
        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
    }
    else if (x > cutRect.fRight - 5 && x < cutRect.fRight + 5 && y < cutRect.fTop + 5 && y > cutRect.fTop - 5) {
        hoverIndex = 2;
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
    }
    else if (x > cutRect.fRight - 5 && x < cutRect.fRight + 5 && y > cutRect.fTop + 5 && y < cutRect.fBottom - 5) {
        hoverIndex = 3;
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
    }
    else if (x > cutRect.fRight - 5 && x < cutRect.fRight + 5 && y > cutRect.fBottom - 5 && y < cutRect.fBottom + 5) {
        hoverIndex = 4;
        SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
    }
    else if (x > cutRect.fLeft + 5 && x < cutRect.fRight - 5 && y < cutRect.fBottom + 5 && y > cutRect.fBottom - 5) {
        hoverIndex = 5;
        SetCursor(LoadCursor(nullptr, IDC_SIZENS));
    }
    else if (x > cutRect.fLeft - 5 && x < cutRect.fLeft + 5 && y < cutRect.fBottom + 5 && y > cutRect.fBottom - 5) {
        hoverIndex = 6;
        SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
    }
    else if (x > cutRect.fLeft - 5 && x < cutRect.fLeft + 5 && y > cutRect.fTop + 5 && y < cutRect.fBottom - 5) {
        hoverIndex = 7;
        SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
    }
    else {
        hoverIndex = -1;
    }
}
