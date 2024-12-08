#include <QEvent>
#include <Windows.h>
#include <QWindow>

#include "WinBase.h"
#include "WinCanvas.h"
#include "WinBoard.h"
#include "../Tool/ToolSub.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeRect.h"
#include "../Shape/ShapeEllipse.h"
#include "../Shape/ShapeArrow.h"
#include "../Shape/ShapeNumber.h"
#include "../Shape/ShapeLine.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeEraserRect.h"
#include "../Shape/ShapeEraserLine.h"
#include "../Shape/ShapeMosaicRect.h"
#include "../Shape/ShapeMosaicLine.h"


WinBase::WinBase(QObject *parent):QObject(parent)
{
}

WinBase::~WinBase()
{
    delete winCanvas;
}

ShapeBase* WinBase::addShape()
{
    for (auto it = shapes.begin(); it != shapes.end(); ) {
        if ((*it)->state == ShapeState::temp) {
            (*it)->deleteLater();
            it = shapes.erase(it);
        }
        else {
            ++it;
        }
    }
    ShapeBase* shape;
    if (state == State::rect) {
        shape = new ShapeRect(this);
    }
    else if (state == State::ellipse) {
        shape = new ShapeEllipse(this);
    }
    else if (state == State::arrow) {
        shape = new ShapeArrow(this);
    }
    else if (state == State::number) {
        shape = new ShapeNumber(this);
    }
    else if (state == State::line) {
        shape = new ShapeLine(this);
    }
    else if (state == State::text) {
        shape = new ShapeText(this);
    }
    else if (state == State::mosaic) {
        auto isRect = toolSub->getSelectState("mosaicFill");
        if (isRect) {
            shape = new ShapeMosaicRect(this);
        }
        else {
            shape = new ShapeMosaicLine(this);
        }
    }
    else if (state == State::eraser) {
        auto isRect = toolSub->getSelectState("eraserFill");
        if (isRect) {
            shape = new ShapeEraserRect(this);
        }
        else {
            shape = new ShapeEraserLine(this);
        }
    }
    else
    {
        return nullptr;
    }
    shapes.push_back(shape);
    return shape;
}

void WinBase::mousePressOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mousePress(event);
    }
    if (!event->isAccepted()) {
        auto shape = addShape();
        shape->mousePress(event); //不然新添加的Shape收不到鼠标按下事件
    }
}

void WinBase::mouseMoveOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseMove(event);
    }
    if (!event->isAccepted()) {
        if (state == State::text) {
            updateCursor(Qt::IBeamCursor);
        }
        else {
            updateCursor(Qt::CrossCursor);
        }
        if (winCanvas) {
            winCanvas->refresh(nullptr);
        }
    }
}

void WinBase::mouseDragOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseDrag(event);
    }
}

void WinBase::mouseReleaseOnShape(QMouseEvent* event)
{
    if (event->isAccepted()) return;
    for (int i = shapes.size() - 1; i >= 0; i--)
    {
        if (event->isAccepted()) return;
        shapes[i]->mouseRelease(event);
    }
    if (winCanvas && !event->isAccepted()) {
        if (state != State::text) {
            winCanvas->refresh(nullptr);
        }
    }
}
void WinBase::initWindow()
{
    regWinClass();
    auto exStyle = WS_EX_LAYERED;  //| WS_EX_TOPMOST | WS_EX_TOOLWINDOW
    auto style = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP;
    hwnd = CreateWindowEx(exStyle,L"ScreenCapture", L"ScreenCapture",style,x, y, w, h, NULL, NULL, GetModuleHandle(NULL), static_cast<LPVOID>(this));
}
void WinBase::showEvent(QShowEvent* event)
{
    auto hwnd = (HWND)winId();
    SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
    QWidget::showEvent(event);
    //auto screens = QGuiApplication::screens();
    //for (auto screen : screens)
    //{
    //    auto pos = screen->geometry().topLeft();
    //    if (pos.x() == 0 && pos.y() == 0)
    //    {
    //        this->setScreen(screen);
    //        break;
    //    }
    //}
}
void WinBase::regWinClass()
{
    static WNDCLASSEX wcx{};
    if (!wcx.lpfnWndProc) {
        auto hinstance = GetModuleHandle(NULL);
        wcx.cbSize = sizeof(wcx);
        wcx.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
        wcx.lpfnWndProc = &WinBase::RouteWinMessage;
        wcx.cbWndExtra = sizeof(WinBase*);
        wcx.hInstance = hinstance;
        wcx.hIcon = LoadIcon(hinstance, IDI_APPLICATION);
        wcx.hCursor = LoadCursor(hinstance, IDC_ARROW);
        wcx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
        wcx.lpszClassName = L"ScreenCapture";
    }
    RegisterClassEx(&wcx);
}
LRESULT WinBase::RouteWinMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (msg == WM_NCCREATE)
    {
        CREATESTRUCT* pCS = reinterpret_cast<CREATESTRUCT*>(lParam);
        LPVOID pThis = pCS->lpCreateParams;
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pThis));
    }
    auto obj = reinterpret_cast<WinBase*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    if(!obj) return DefWindowProc(hWnd, msg, wParam, lParam);
    switch (msg)
    {
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE)
            {
                return false;
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            IsMouseDown = true;
            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);
            return onMouseDown(x, y);
        }
        case WM_LBUTTONDBLCLK:
        {
            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);
            return onDoubleClick(x, y);
        }
        case WM_LBUTTONUP:
        {
            IsMouseDown = false;
            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);
            return onMouseUp(x, y);
        }
        case WM_MOUSEMOVE:
        {
            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);
            if (IsMouseDown) {
                return onMouseDrag(x, y);
            }
            else {
                return onMouseMove(x, y);
            }
        }
        case WM_RBUTTONDOWN:
        {
            auto x = GET_X_LPARAM(lparam);
            auto y = GET_Y_LPARAM(lparam);
            onMouseDownRight(x, y);
            return true;
        }
        default:
        {
            return obj->wndProc(hWnd, msg, wParam, lParam);
        }
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WinBase::wndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
void WinBase::updateCursor(Qt::CursorShape cur)
{
    //if (cursor().shape() != cur) {
    //    setCursor(cur);
    //}
}
void WinBase::refreshBoard()
{
    winBoard->refresh();
}
void WinBase::refreshCanvas(ShapeBase* shape,bool force)
{
    winCanvas->refresh(shape, force);
}

void WinBase::show()
{
    paint();
    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);
    SetCursor(LoadCursor(nullptr, IDC_ARROW));
}

void WinBase::paint()
{
    HDC hdc = GetDC(hwnd);
    auto compDC = CreateCompatibleDC(NULL);
    auto bitmap = CreateCompatibleBitmap(hdc, w, h);
    DeleteObject(SelectObject(compDC, bitmap));

    BITMAPINFO info = { sizeof(BITMAPINFOHEADER), w, 0 - h, 1, 32, BI_RGB, w * 4 * h, 0, 0, 0, 0 };
    SetDIBits(hdc, bitmap, 0, h, img.bits(), &info, DIB_RGB_COLORS);
    BLENDFUNCTION blend = { .BlendOp{AC_SRC_OVER}, .SourceConstantAlpha{255}, .AlphaFormat{AC_SRC_ALPHA} };
    POINT pSrc = { 0, 0 };
    SIZE sizeWnd = { w, h };
    UpdateLayeredWindow(hwnd, hdc, NULL, &sizeWnd, compDC, &pSrc, NULL, &blend, ULW_ALPHA);
    ReleaseDC(hwnd, hdc);

    DeleteDC(compDC);
    DeleteObject(bitmap);
}
