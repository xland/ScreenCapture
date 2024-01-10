#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#include "CutMask.h"
#include "WindowBase.h"
#include "App.h"
#include "include/core/SkColor.h"
#include "State.h"
#include "Icon.h"
#include "Cursor.h"
#include <format>
#include "ToolMain.h"

CutMask* cutMask;

CutMask::CutMask()
{
    
}

void CutMask::setPath()
{
    path.reset();
    path.addRect(CutRect);
    path.setFillType(SkPathFillType::kInverseWinding);
    App::GetWin()->Refresh();
}

CutMask::~CutMask()
{
}

void CutMask::Init()
{
    if (!cutMask)
    {
        cutMask = new CutMask();
    }
}

CutMask* CutMask::Get()
{
    return cutMask;
}

SkRect CutMask::GetCutRect()
{
    auto rect = cutMask->CutRect;
    rect.inset(2, 2);
    return rect;
}

bool CutMask::OnMouseDown(const int& x, const int& y)
{    
    auto win = App::GetWin();
    if (win->state == State::start) {
        hoverIndex = 4;
        CutRect.setXYWH(x, y, 0, 0);
    }
    if (hoverIndex == 0) {
        CutRect.setLTRB(x, y, CutRect.fRight, CutRect.fBottom);
        start.set(CutRect.fRight, CutRect.fBottom); 
    }
    else if (hoverIndex == 1) {
        CutRect.setLTRB(CutRect.fLeft, y, CutRect.fRight, CutRect.fBottom); 
        start.set(0, CutRect.fBottom);
    }
    else if (hoverIndex == 2) {
        CutRect.setLTRB(CutRect.fLeft, y, x, CutRect.fBottom); 
        start.set(CutRect.fLeft, CutRect.fBottom);
    }
    else if (hoverIndex == 3) {
        CutRect.setLTRB(CutRect.fLeft, CutRect.fTop, x, CutRect.fBottom);
        start.set(CutRect.fLeft, 0);
    }
    else if (hoverIndex == 4) {
        CutRect.setLTRB(CutRect.fLeft, CutRect.fTop, x, y);
        start.set(CutRect.fLeft, CutRect.fTop);
    }
    else if (hoverIndex == 5) {
        CutRect.setLTRB(CutRect.fLeft, CutRect.fTop, CutRect.fRight, y);
        start.set(0, CutRect.fTop);
    }
    else if (hoverIndex == 6) {
        CutRect.setLTRB(x, CutRect.fTop, CutRect.fRight, y); 
        start.set(CutRect.fRight, CutRect.fTop);
    }
    else if (hoverIndex == 7) {
        CutRect.setLTRB(x, CutRect.fTop, CutRect.fRight, CutRect.fBottom);
        start.set(CutRect.fRight, 0);
    }
    else if (hoverIndex == 8) {
        start.set(x - CutRect.fLeft, y - CutRect.fTop);
        return true;
    }
    else {
        return false;
    }
    win->surfaceFront->getCanvas()->clear(SK_ColorTRANSPARENT);
    win->state = State::mask;
    setPath();
    return true;
}
bool CutMask::OnMouseMove(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state < State::mask)
    {
        for (size_t i = 0; i < winRects.size(); i++)
        {
            if (winRects[i].contains(x, y)) {
                CutRect = winRects[i];
                setPath();
                return false;
            }
        }
        return false;
    }
    if (win->state == State::tool) {
        if (x < CutRect.fLeft && y < CutRect.fTop) {
            hoverIndex = 0;
            Cursor::LeftTopRightBottom();
        }
        else if (x > CutRect.fLeft && x < CutRect.fRight && y < CutRect.fTop) {
            hoverIndex = 1;
            Cursor::TopBottom();
        }
        else if (x > CutRect.fRight && y < CutRect.fTop) {
            hoverIndex = 2;
            Cursor::LeftBottomRightTop();
        }
        else if (x > CutRect.fRight && y > CutRect.fTop && y < CutRect.fBottom) {
            hoverIndex = 3;
            Cursor::LeftRight();
        }
        else if (x > CutRect.fRight && y > CutRect.fBottom) {
            hoverIndex = 4;
            Cursor::LeftTopRightBottom();
        }
        else if (x > CutRect.fLeft && x < CutRect.fRight && y > CutRect.fBottom) {
            hoverIndex = 5;
            Cursor::TopBottom();
        }
        else if (x < CutRect.fLeft && y > CutRect.fBottom) {
            hoverIndex = 6;
            Cursor::LeftBottomRightTop();
        }
        else if (x < CutRect.fLeft && y < CutRect.fBottom && y > CutRect.fTop) {
            hoverIndex = 7;
            Cursor::LeftRight();
        }
        else {
            hoverIndex = 8;
            Cursor::All();
        }
        return true;
    }
    else {
        if (x > CutRect.fLeft - 5 &&x < CutRect.fLeft+5 && y < CutRect.fTop+5 && y > CutRect.fTop - 5) {
            hoverIndex = 0;
            Cursor::LeftTopRightBottom();
            return true;
        }
        else if (x > CutRect.fLeft + 5 && x < CutRect.fRight - 5 && y < CutRect.fTop + 5 && y > CutRect.fTop - 5) {
            hoverIndex = 1;
            Cursor::TopBottom();
            return true;
        }
        else if (x > CutRect.fRight - 5 && x < CutRect.fRight + 5 && y < CutRect.fTop + 5 && y > CutRect.fTop - 5) {
            hoverIndex = 2;
            Cursor::LeftBottomRightTop();
            return true;
        }
        else if (x > CutRect.fRight - 5 && x < CutRect.fRight + 5 && y > CutRect.fTop + 5 && y < CutRect.fBottom - 5) {
            hoverIndex = 3;
            Cursor::LeftRight();
            return true;
        }
        else if (x > CutRect.fRight - 5 && x < CutRect.fRight + 5 && y > CutRect.fBottom - 5 && y < CutRect.fBottom + 5) {
            hoverIndex = 4;
            Cursor::LeftTopRightBottom();
            return true;
        }
        else if (x > CutRect.fLeft + 5 && x < CutRect.fRight - 5 && y < CutRect.fBottom + 5 && y > CutRect.fBottom - 5) {
            hoverIndex = 5;
            Cursor::TopBottom();
            return true;
        }
        else if (x > CutRect.fLeft - 5 && x < CutRect.fLeft + 5 && y < CutRect.fBottom + 5 && y > CutRect.fBottom - 5) {
            hoverIndex = 6;
            Cursor::LeftBottomRightTop();
            return true;
        }
        else if (x > CutRect.fLeft - 5 && x < CutRect.fLeft + 5 && y > CutRect.fTop + 5 && y < CutRect.fBottom - 5) {
            hoverIndex = 7;
            Cursor::LeftRight();
            return true;
        }
        else {
            hoverIndex = -1;
            return false;
        }
    }    
}
bool CutMask::OnMouseDrag(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (hoverIndex < 0) {
        return false;
    }
    win->state = State::mask;
    if (hoverIndex == 0) {
        CutRect.setLTRB(x, y, start.fX, start.fY);
    }
    else if (hoverIndex == 1) {
        CutRect.setLTRB(CutRect.fLeft, y, CutRect.fRight, start.y());
    }
    else if (hoverIndex == 2) {
        CutRect.setLTRB(start.fX, y, x, start.fY);
    }
    else if (hoverIndex == 3) {
        CutRect.setLTRB(start.fX, CutRect.fTop, x, CutRect.fBottom);
    }
    else if (hoverIndex == 4) {
        CutRect.setLTRB(start.fX, start.fY, x, y);
    }
    else if (hoverIndex == 5) {
        CutRect.setLTRB(CutRect.fLeft, start.fY, CutRect.fRight, y);
    }
    else if (hoverIndex == 6) {
        CutRect.setLTRB(x, start.fY, start.fX, y);
    }
    else if (hoverIndex == 7) {
        CutRect.setLTRB(x, CutRect.fTop, start.fX, CutRect.fBottom);
    }
    else if (hoverIndex == 8) {
        auto left = x - start.fX;
        auto top = y - start.fY;
        if (left < 0) left = 0;
        if (top < 0) top = 0;
        auto right = left + CutRect.width();
        auto bottom = top + CutRect.height();
        if (right > win->w) {
            right = win->w;
            left = win->w - CutRect.width();
        }
        if (bottom > win->h) {
            bottom = win->h;
            top = win->h - CutRect.height();
        }
        CutRect.setLTRB(left, top, right, bottom);
        setPath();
        start.set(x - CutRect.fLeft, y - CutRect.fTop);
        return true;
    }
    CutRect.sort();
    setPath();    
    return true;
}
bool CutMask::OnPaint(SkCanvas *canvas)
{
    auto win = App::GetWin();
    SkPaint paint;
    paint.setColor(SkColorSetARGB(160, 0, 0, 0));
    canvas->drawPath(path, paint);
    paint.setColor(SkColorSetARGB(255, 22, 118, 255));
    paint.setStrokeWidth(3);
    paint.setStyle(SkPaint::Style::kStroke_Style);
    canvas->drawRect(CutRect, paint);
    auto font = App::GetFontText();
    auto str = std::format("Left:{}  Top:{}  Right:{}  Bottom:{}  Width:{}  Height:{}", CutRect.fLeft, CutRect.fTop, CutRect.fRight,
        CutRect.fBottom, CutRect.width(), CutRect.height());
    font->setSize(14);
    auto data = str.data();
    SkRect rectTemp;
    font->measureText(data, str.size(), SkTextEncoding::kUTF8, &rectTemp);
    SkRect rectInfo = SkRect::MakeXYWH(CutRect.fLeft, CutRect.fTop - 36, rectTemp.width()+16, 32);
    if (CutRect.fTop < 38) {
        rectInfo.offset(6, 42);
    }
    paint.setAntiAlias(true);
    paint.setStroke(false);
    paint.setColor(SkColorSetARGB(130, 0, 0, 0));
    canvas->drawRoundRect(rectInfo,3,3,paint);
    paint.setColor(SkColorSetARGB(255, 220, 220, 220));
    canvas->drawSimpleText(data, str.size(), SkTextEncoding::kUTF8, rectInfo.fLeft + 8, rectInfo.fTop + 21, *font, paint);
    return false;
}
bool CutMask::OnKeyDown(const unsigned int& val)
{
    auto win = App::GetWin();
    if (win->state < State::mask)
    {
        return false;
    }
    auto left{ CutRect.fLeft }, top{ CutRect.fTop }, right{ CutRect.fRight }, bottom{ CutRect.fBottom };    
    if (val == VK_UP) {
        if (top <= 0) {
            return false;
        }
        top -= 1;
        bottom -= 1;
    }
    else if (val == VK_DOWN) {
        if (bottom >= win->h) {
            return false;
        }
        top += 1;
        bottom += 1;
    }
    else if (val == VK_LEFT) {
        if (left <= 0) {
            return false;
        }
        left -= 1;
        right -= 1;
    }
    else if (val == VK_RIGHT) {
        if (right >= win->w) {
            return false;
        }
        left += 1;
        right += 1;
    }
    return false;
}
bool CutMask::OnMouseUp(const int& x, const int& y)
{
    auto win = App::GetWin();
    if (win->state != State::mask) {
        return false;
    }
    win->state = State::tool;
    auto tool = ToolMain::Get();
    tool->Reset();
    win->Refresh();
    Cursor::Arrow();
    return true;
}

void CutMask::EnumWinRects()
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