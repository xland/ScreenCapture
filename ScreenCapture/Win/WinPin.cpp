#include "pch.h"
#include "Util.h"
#include "App.h"
#include "WinPin.h"
#include "WinCap.h"
#include "CutMask.h"
#include "WinToolMain.h"
#include "WinToolSub.h"
#include "History.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"
#include "Shape/ShapeEllipse.h"
#include "Shape/ShapeArrow.h"
#include "Shape/ShapeNumber.h"
#include "Shape/ShapeLine.h"
#include "Shape/ShapeText.h"
#include "Shape/ShapeMosaic.h"
#include "Shape/ShapeEraser.h"

std::vector<std::unique_ptr<WinPin>> winPins;

WinPin::WinPin(const int& x, const int& y, const int& w, const int& h) :
    WinBase(x, y, w, h), history{std::make_unique<History>(this)}
{
}

WinPin::~WinPin()
{
}


void WinPin::init()
{
    auto cap = WinCap::get();
    auto& r = cap->cutMask->maskRect;
	auto win = std::make_unique<WinPin>((int)r.left, (int)r.top, (int)(r.right - r.left), (int)(r.bottom - r.top));
    win->createWindow(WS_EX_TOOLWINDOW | WS_EX_TOPMOST| WS_EX_NOACTIVATE, WS_POPUP);
    win->initImg();
    win->enableShadow();
    win->show();
    WinToolMain::popup(win.get());
	winPins.push_back(std::move(win));
}

WinPin* WinPin::getCur()
{
    auto tarHwnd = GetActiveWindow();
    for (const auto& pinPtr : winPins) {
        if (pinPtr->hwnd == tarHwnd) { 
			return pinPtr.get();
        }
    }
	return nullptr;
}

void WinPin::onPaint()
{
    D2D1_RECT_F destRect = D2D1::RectF(0,0, (float)w, (float)h);
    render->DrawBitmap(screenImg.Get(), destRect);
    for (auto& shape:history->shapes)
    {
        if (!shape->isUndo) {
            shape->paint();
        }
    }
    if (!isMouseDown && shapeHover) {
        shapeHover->paintDragger();
    }
}

bool WinPin::onCursor()
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") {
        setCursor(IDC_SIZEALL);
        return TRUE;
    }
    if (shapeHover) {
        shapeHover->setCursor();
        return TRUE;
    }
    if (toolMain->state == "text") {
        setCursor(IDC_IBEAM);
    }
    else {
        setCursor(IDC_CROSS);
    }
    return TRUE;
}

void WinPin::onTimer(const UINT& timerId)
{
    if (timerId != 18) return;
    if (!shapeHover) {
        refresh();
        killTimer(timerId);
    }
}

void WinPin::onKeyDown(const TCHAR& key)
{
    if (key == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        history->undo();
    }
    else if (key == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        history->redo();
    }
}

void WinPin::initImg()
{
    auto win = WinCap::get();
    auto props = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
    auto rect = D2D1::RectU(x, y, x+w, y+h);
    auto cpuImg = win->getImgByRect(rect);
    D2D1_MAPPED_RECT mapped{};
    auto hr = cpuImg->Map(D2D1_MAP_OPTIONS_READ, &mapped);
    hr = render->CreateBitmap(cpuImg->GetPixelSize(), mapped.bits, mapped.pitch, &props, screenImg.GetAddressOf());
    hr = cpuImg->Unmap();
}


void WinPin::onMouseMove(const int& x, const int& y)
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") return;
    int i{ (int)(history->shapes.size() - 1) };
    for (; i >= 0; i--)
    {
        auto cur = history->shapes[i].get();
        if (cur->isUndo) continue;
        cur->mouseMove((float)x, (float)y);
        if (cur->hoverDraggerIndex>=0) {
            if (shapeHover != cur) {
                shapeHover = cur;
                setTimer(800, 18);
                refresh();
            }
            return;
        }
    }
    if (shapeHover) {
        shapeHover = nullptr;
    }
}

void WinPin::onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers)
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") {
        this->x += (x - pressPos.x);
        this->y += (y - pressPos.y);
        move(this->x, this->y);
    }
    else if (toolMain->state != "text") {
        shapeHover->mouseDrag((float)x, (float)y,modifiers);
        refresh();
    }
}

void WinPin::onMouseDown(const int& x, const int& y, bool isRight)
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") {
        pressPos.x = x;
        pressPos.y = y;
        toolMain->hide();
        return;
    }
    if (shapeHover) {
        shapeHover->mouseDown((float)x, (float)y);
        return;
    }
    shapeHover = history->createShape(toolMain->state, x, y);
}

void WinPin::onMouseUp(const int& x, const int& y)
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") {
        WinToolMain::get()->popup(this);
    }
    else if(shapeHover) {
        shapeHover->mouseUp((float)x, (float)y);
        refresh();
        setTimer(800,18);
    }
}

void WinPin::onMouseLeave()
{

}
