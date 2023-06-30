#include "MainWin.h"
#include <memory>


MainWin::MainWin(HINSTANCE hinstance):hinstance{hinstance},
    direct2dFactory{nullptr},hwnd{nullptr}
{
    shotScreen();
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &direct2dFactory);
    createWindow();
}
MainWin::~MainWin()
{
    SafeRelease(&direct2dFactory);
    SafeRelease(&render);
    SafeRelease(&brush);
    SafeRelease(&maskBrush);
    SafeRelease(&bgImg);    
}

void MainWin::Paint()
{
    createDeviceRes();
    render->BeginDraw();
    render->SetTransform(D2D1::Matrix3x2F::Identity());
    render->DrawBitmap(bgImg,D2D1::RectF(x,y,w/scaleFactor,h/scaleFactor));
    //renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::White));
    render->DrawLine(D2D1::Point2F(0.0f, 0.0f),D2D1::Point2F(w, h),brush,3.5f);
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(1100.f, 1100.f), 200.f, 350.f);
    render->DrawEllipse(ellipse, brush, 10.f);
    for (size_t i = 0; i < 8; i++)
    {
        render->FillRectangle(masks[i], maskBrush);
    }   

    HRESULT hr = render->EndDraw();
    if (hr == D2DERR_RECREATE_TARGET)
    {
        hr = S_OK;
        SafeRelease(&render);
        SafeRelease(&brush);
        SafeRelease(&maskBrush);
    }
}
void MainWin::createDeviceRes()
{
    if (render) return;
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    direct2dFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush);
    render->CreateSolidColorBrush(D2D1::ColorF(0,0,0,0.68), &maskBrush);
    D2D1_BITMAP_PROPERTIES bmpPorp;
    bmpPorp.dpiX = 0.0f;
    bmpPorp.dpiY = 0.0f;
    bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    render->CreateBitmap(D2D1::SizeU(w, h), bgPixels, 4 * w, bmpPorp, &bgImg);
    delete[] bgPixels;
}

template<class Interface>
void MainWin::SafeRelease(Interface** ppInterfaceToRelease)
{
    if (*ppInterfaceToRelease != NULL)
    {
        (*ppInterfaceToRelease)->Release();
        (*ppInterfaceToRelease) = NULL;
    }
}