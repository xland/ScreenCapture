#include "pch.h"
#include "Util.h"
#include "App.h"
#include "WinPin.h"
#include "WinCap.h"
#include "CutMask.h"
#include "WinToolMain.h"
#include "WinToolSub.h"
#include "Shape/ShapeBase.h"
#include "Shape/ShapeRect.h"

std::vector<std::unique_ptr<WinPin>> winPins;

WinPin::WinPin(const int& x, const int& y, const int& w, const int& h) :WinBase(x, y, w, h)
{
}

WinPin::~WinPin()
{
}


void WinPin::init()
{
    auto cap = WinCap::get();
    auto& r = cap->cutMask->maskRect;
	auto win = std::make_unique<WinPin>(r.left, r.top, r.right - r.left, r.bottom - r.top);
    win->createWindow(WS_EX_TOOLWINDOW | WS_EX_TOPMOST, WS_POPUP);
    win->initImg();
    win->enableShadow();
    win->show();
	winPins.push_back(std::move(win));
    WinToolMain::popup();
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
    D2D1_RECT_F destRect = D2D1::RectF(0,0, w, h);
    render->DrawBitmap(screenImg.Get(), destRect);
    for (auto& shape:shapes)
    {
        shape->paint();
    }
}

bool WinPin::onCursor()
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") {
        SetCursor(LoadCursor(NULL, IDC_SIZEALL));
    }
    else {
        SetCursor(LoadCursor(NULL, IDC_CROSS));
    }
    return TRUE;
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
}

void WinPin::onMouseDrag(const int& x, const int& y)
{
    auto toolMain = WinToolMain::get();
    if (toolMain->state == "") {
        this->x += (x - pressPos.x);
        this->y += (y - pressPos.y);
        SetWindowPos(hwnd, nullptr, this->x, this->y, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED);
    }
    else if (toolMain->state == "rect") {
        shapes[shapes.size() - 1]->mouseDrag(x, y);
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
    }
    else if(toolMain->state == "rect"){
        auto rect = std::make_unique<ShapeRect>(this);
        rect->mouseDown(x, y);
        shapes.push_back(std::move(rect));
    }
}

void WinPin::onMouseUp(const int& x, const int& y)
{
    WinToolMain::get()->popup();
}

void WinPin::onMouseLeave()
{

}
