#include "WindowBase.h"
#include <dwmapi.h>

LRESULT CALLBACK WindowBase::RouteWindowMessageWhenInit(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_INITDIALOG)
    {
        WindowBase* dialog = (WindowBase*)lParam;
        dialog->hwnd = hWnd;
        SetWindowLongPtr(hWnd, DWLP_USER, (LONG_PTR)dialog);
        SetWindowLongPtr(hWnd, DWLP_DLGPROC, (LONG_PTR)WindowBase::RouteWindowMessage);
        return dialog->WindowProc(hWnd,msg, wParam, lParam);
    }
    return FALSE;  

}

LRESULT CALLBACK WindowBase::RouteWindowMessage(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    WindowBase* dialog = (WindowBase*)GetWindowLongPtr(hWnd, DWLP_USER);
    return dialog->WindowProc(hWnd, msg, wParam, lParam);
}

WindowBase::WindowBase()
{
}

WindowBase::~WindowBase()
{
    delete bgImage;
    delete d2DImage;
    delete render;
    delete factory;
}

void WindowBase::Show()
{

}
void WindowBase::Close()
{

}

void WindowBase::InitWindow(const int& x, const int& y, const unsigned int& w, const unsigned int& h)
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
    bgImage = new BLImage(w, h, BL_FORMAT_PRGB32);


    struct MyDialog : DLGTEMPLATE {
        WORD dummy[3] = { 0 };  // unused menu, class and title
    }
    dlg;
    dlg.style = WS_POPUP | WS_CAPTION | DS_CENTER;//WS_OVERLAPPEDWINDOW;//
    dlg.dwExtendedStyle = 0;
    dlg.cdit = 0;
    dlg.x = x;
    dlg.y = y;
    dlg.cx = w;//width
    dlg.cy = h;//height
    auto hinstance = GetModuleHandle(NULL);
    //this is model,modeless use CreateDialogIndirectParam
    DialogBoxIndirectParam(hinstance, &dlg, nullptr, WindowBase::RouteWindowMessageWhenInit, (LPARAM)this);
}

void WindowBase::d2DCreateRes()
{
    if (render) return;
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
    render->SetTransform(D2D1::Matrix3x2F::Identity());
    D2D1_BITMAP_PROPERTIES bmpPorp;
    bmpPorp.dpiX = 0;
    bmpPorp.dpiY = 0;
    bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    BLImageData imgData;
    bgImage->getData(&imgData);
    render->CreateBitmap(size, imgData.pixelData, imgData.stride, bmpPorp, &d2DImage);
}

LRESULT CALLBACK WindowBase::WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
        case WM_INITDIALOG:
        {
            MARGINS m{ 0, 0, 0, 1 };
            DwmExtendFrameIntoClientArea(hWnd, &m);
            SetWindowPos(hWnd, nullptr, 0, 0, 0, 0, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
            return TRUE;
        }
        case WM_NCCALCSIZE:
        {
            if (wParam == TRUE)
            {
                SetWindowLong(hWnd, DWLP_MSGRESULT, 0);
                return TRUE;
            }
            return FALSE;
        }
        case WM_PAINT:
        {
            d2DCreateRes();
            render->BeginDraw();
            render->DrawBitmap(d2DImage, D2D1::RectF(0, 0, w, h));
            auto result = render->EndDraw();
            if (D2DERR_RECREATE_TARGET == result)
            {
                d2DImage->Release();
                render->Release();
                render = nullptr;
            }
            ValidateRect(hwnd, NULL);
            return TRUE;
        }
        case WM_NCHITTEST:
        {
            SetWindowLong(hWnd, DWLP_MSGRESULT, HTCAPTION);
            return TRUE;
        }
        case WM_COMMAND:
        {
            WORD id = LOWORD(wParam);
            if (id == IDOK || id == IDCANCEL)
            {
                EndDialog(hWnd, id);
                return TRUE;
            }
            return FALSE;
        }
    }
    return FALSE; // return FALSE to let DefDialogProc handle the message
}