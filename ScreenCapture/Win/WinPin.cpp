#include "pch.h"
#include "Util.h"
#include "App.h"
#include "WinPin.h"
#include "WinCap.h"
#include "WinCutMask.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
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

static constexpr int timerID{ 18 };
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
    win->createWindow(WS_EX_TOPMOST| WS_EX_NOACTIVATE);
    win->initImg();
    win->enableShadow();
    win->show();
    win->initTool();
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

void WinPin::initFromData(int x, int y, int w, int h, std::vector<BYTE>& data)
{
    auto win = std::make_unique<WinPin>(x, y, w, h);
    win->createWindow(WS_EX_TOPMOST | WS_EX_NOACTIVATE);
    win->initImgFromData(data);
    win->enableShadow();
    win->show();
    win->initTool();
    winPins.push_back(std::move(win));
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

BOOL WinPin::onCursor()
{
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
    if (timerId != timerID) return;
    if (!shapeHover) {
        refresh();
        killTimer(timerId);
    }
}

void WinPin::onKeyDown(const UINT& key)
{
    if (key == 'Z' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        history->undo();
    }
    else if (key == 'Y' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        history->redo();
    }
    else if (key == 'C' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        copyToClipboard();
    }
    else if (key == 'S' && (GetKeyState(VK_CONTROL) & 0x8000) != 0)
    {
        saveToFile();
    }
}

void WinPin::onDestroy()
{
    if (toolSub.get()) {
        toolSub->close();
    }
    if (toolMain.get()) {
        toolMain->close();
    }
    winPins.erase(std::remove_if(winPins.begin(), winPins.end(), [this](auto& p) { return p.get() == this; }),  winPins.end());
    if (App::getArg(L"auto-quit") == L"true") {
        App::exit(0);
    }
}

void WinPin::onDpiChanged()
{
    toolMain->move(x, y + h + 5.f * dpi);
    if (toolMain->state != "") {
        //toolSub->move(toolMain->x, toolMain->y + toolMain->h + 3.f * toolSub->dpi + 0.5);
        toolSub->resetVal();
    }
}

void WinPin::copyToClipboard()
{
    std::vector<BYTE> pixels;
    if (!getImagePixels(pixels)) return;
    Util::saveToClipboard(w, h, pixels.data());
    close();
}

void WinPin::saveToFile()
{
    auto foregroundBeforeDialog = GetForegroundWindow();
    auto path = Util::getSaveFilePath(hwnd);
    if (path.empty()) {
        restoreWindowState(foregroundBeforeDialog);
        return;
    }
    std::vector<BYTE> pixels;
    if (!getImagePixels(pixels)) {
        restoreWindowState(foregroundBeforeDialog);
        return;
    }
    if (Util::saveToFile(path, w, h, pixels.data())) {
        close();
    }
    else {
        restoreWindowState(foregroundBeforeDialog);
    }
}

bool WinPin::getImagePixels(std::vector<BYTE>& pixels)
{
    if (w <= 0 || h <= 0) return false;
    auto size = D2D1::SizeU((UINT32)w, (UINT32)h);

    // 直接以 screenImg 为渲染目标，把尚未撤销的 shape 叠画到背景上
    render->SetTarget(screenImg.Get());
    render->SetTransform(D2D1::Matrix3x2F::Identity());
    render->BeginDraw();
    for (auto& shape : history->shapes) {
        if (!shape->isUndo) {
            shape->paint();
        }
    }
    auto hr = render->EndDraw();
    if (FAILED(hr)) return false;

    // EndDraw 后 screenImg 仍挂在 render target 上，必须先解绑才能作为 CopyFromBitmap 的 source
    render->SetTarget(nullptr);

    // 创建 CPU 可读副本，从 screenImg 拷贝像素
    D2D1_BITMAP_PROPERTIES1 cpuProps{};
    cpuProps.pixelFormat = screenImg->GetPixelFormat();
    cpuProps.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    cpuProps.dpiX = 96.0f;
    cpuProps.dpiY = 96.0f;
    ComPtr<ID2D1Bitmap1> cpuBmp;
    hr = render->CreateBitmap(size, nullptr, 0, &cpuProps, cpuBmp.GetAddressOf());
    if (FAILED(hr)) return false;

    hr = cpuBmp->CopyFromBitmap(nullptr, screenImg.Get(), nullptr);
    if (FAILED(hr)) return false;

    D2D1_MAPPED_RECT mapped{};
    hr = cpuBmp->Map(D2D1_MAP_OPTIONS_READ, &mapped);
    if (FAILED(hr)) return false;

    // D2D CPU 位图的 mapped.pitch 按 GPU 行对齐，可能 > w*4
    // 剪切板和 WIC PNG 都使用紧凑的 w*4 行步长，必须逐行紧缩
    UINT32 rowBytes = (UINT32)w * 4;
    pixels.resize((size_t)rowBytes * h);
    for (int row = 0; row < h; ++row) {
        CopyMemory(
            pixels.data() + (size_t)row * rowBytes,
            mapped.bits   + (size_t)row * mapped.pitch,
            rowBytes);
    }
    cpuBmp->Unmap();
    return true;
}

void WinPin::initImg()
{
    auto win = WinCap::get();
    auto rect = D2D1::RectU(x, y, x+w, y+h);
    auto cpuImg = win->getImgByRect(rect);
    D2D1_MAPPED_RECT mapped{};
    auto hr = cpuImg->Map(D2D1_MAP_OPTIONS_READ, &mapped); //todo error check
    // 创建带 TARGET 标志的 Bitmap1，使其可直接作为渲染目标
    D2D1_BITMAP_PROPERTIES1 props{};
    props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    hr = render->CreateBitmap(cpuImg->GetPixelSize(), mapped.bits, mapped.pitch, &props, screenImg.GetAddressOf());
    hr = cpuImg->Unmap();
}

void WinPin::initImgFromData(std::vector<BYTE>& data)
{
    D2D1_BITMAP_PROPERTIES1 props{};
    props.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
    props.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
    props.dpiX = 96.0f;
    props.dpiY = 96.0f;
    render->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, &props, screenImg.GetAddressOf());
}

void WinPin::initTool()
{
    auto btnSize{ 32.f * dpi };
    auto toolStyle{ WS_EX_TOPMOST | WS_EX_NOACTIVATE };
    toolMain = std::make_unique<ToolMain>(x, y + h + 5.f * dpi, btnSize * 13, btnSize,this);
    toolMain->createWindow(toolStyle);
    toolMain->show();
    toolSub = std::make_unique<ToolSub>(this);
    toolSub->createWindow(toolStyle);
}

void WinPin::restoreWindowState(HWND foregroundBeforeDialog)
{
    // IFileSaveDialog 关闭后会把 owner(hwnd) 恢复为活动窗口。
        // 窗口未关闭时恢复打开对话框之前的前台窗口状态，并重新把工具栏放回贴图窗口之上，
        // 避免 WinPin 被激活后阴影盖住 ToolMain。
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    if (toolMain.get() && toolMain->hwnd && IsWindow(toolMain->hwnd) && IsWindowVisible(toolMain->hwnd)) {
        SetWindowPos(toolMain->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
        toolMain->syncHoverWithCursor();
    }
    if (toolSub.get() && toolSub->hwnd && IsWindow(toolSub->hwnd) && IsWindowVisible(toolSub->hwnd)) {
        SetWindowPos(toolSub->hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
    }
    if (foregroundBeforeDialog && foregroundBeforeDialog != hwnd && IsWindow(foregroundBeforeDialog) && IsWindowVisible(foregroundBeforeDialog)) {
        SetForegroundWindow(foregroundBeforeDialog);
    }
}


void WinPin::onMouseMove(const int& x, const int& y)
{
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
                setTimer(800, timerID);
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
    if (toolMain->state == "") {
        this->x += (x - pressPos.x);
        this->y += (y - pressPos.y);
        move(this->x, this->y);
    }
    else{
        shapeHover->mouseDrag((float)x, (float)y,modifiers);
        refresh();
    }
}

void WinPin::onMouseDown(const int& x, const int& y, bool isRight)
{
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

void WinPin::onMouseDoubleClick(const int& x, const int& y, bool isRight)
{
    if (isRight) return;
    copyToClipboard();
}

void WinPin::onMouseUp(const int& x, const int& y)
{
    if (toolMain->state == "") { //state为空时，是在拖动窗口
        toolMain->move(this->x, this->y + h + 5.f * dpi);
        toolMain->show();
    }
    else if(shapeHover) {
        shapeHover->mouseUp((float)x, (float)y);
        refresh();
        setTimer(800, timerID);
    }
}

void WinPin::onMouseLeave()
{

}

void WinPin::onMouseWheel(const int& x, const int& y, const short& delta)
{
    if (shapeHover) {
        shapeHover->mouseWheel(x,y,delta);
    }
}
