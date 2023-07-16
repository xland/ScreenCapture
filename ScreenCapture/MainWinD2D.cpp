#include "MainWin.h"

void MainWin::d2DCreateFactory()
{
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
}

void MainWin::d2DCreateRes()
{
    if (render) return;
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
    render->SetTransform(D2D1::Matrix3x2F::Identity());
    D2D1_BITMAP_PROPERTIES bmpPorp;
    bmpPorp.dpiX = 0;
    bmpPorp.dpiY = 0;
    bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;// DXGI_FORMAT_B8G8R8A8_UNORM; DXGI_FORMAT_R8G8B8A8_UNORM
    bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    BLImageData imgData;
    bgImage->getData(&imgData);
    render->CreateBitmap(size, imgData.pixelData, imgData.stride, bmpPorp, &d2DImage);
    w = w / scaleFactor;
    h = h / scaleFactor;
}

void MainWin::d2DDraw()
{
    d2DCreateRes();
    render->BeginDraw();
    render->DrawBitmap(d2DImage, D2D1::RectF(0, 0, w, h));
    auto result = render->EndDraw();
    if (D2DERR_RECREATE_TARGET == result)
    {
        w = w * scaleFactor;
        h = h * scaleFactor;
        d2DImage->Release();
        render->Release();
        render = nullptr;
    }
}