#include "MainWin.h"

void MainWin::initCanvas()
{
	D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory);
}

void MainWin::paint()
{
    createDeviceRes();
    render->BeginDraw();
    render->SetTransform(D2D1::Matrix3x2F::Identity());
    render->DrawBitmap(bgImg, D2D1::RectF(0, 0, w, h));
    render->DrawLine(D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(w, h), brush, 3.5f);
    D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(500.f, 500.f), 200.f, 350.f);
    render->DrawEllipse(ellipse, brush, 10.f);
    paintMask();
    auto result = render->EndDraw();
    paintError(result);
}

void MainWin::createDeviceRes()
{
    if (render) return;
    
    D2D1_SIZE_U size = D2D1::SizeU(w, h);
    factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hwnd, size), &render);
    render->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &brush);
    render->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.68), &maskBrush); 
    render->CreateLayer(NULL, &maskLayer);

    

    D2D1_BITMAP_PROPERTIES bmpPorp;
    bmpPorp.dpiX = 0.0f;
    bmpPorp.dpiY = 0.0f;
    bmpPorp.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
    bmpPorp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
    render->CreateBitmap(size, bgPixels, 4 * w, bmpPorp, &bgImg);   

    w = w / scaleFactor;
    h = h / scaleFactor;
}

void MainWin::paintMask()
{
    auto param = D2D1::LayerParameters(D2D1::InfiniteRect(),NULL,
        D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,D2D1::IdentityMatrix(),
        1.0,NULL,D2D1_LAYER_OPTIONS_NONE);
    render->PushLayer(param,maskLayer);
    render->FillRectangle(D2D1::RectF(0, 0, w, h), maskBrush);
    render->PushAxisAlignedClip(cutRect, D2D1_ANTIALIAS_MODE_ALIASED);
    render->Clear(D2D1::ColorF(0, 0, 0, 0));
    render->PopAxisAlignedClip();
    render->PopLayer();
}
void MainWin::paintError(const HRESULT& result)
{
    if (D2DERR_RECREATE_TARGET == result)
    {
        initScaleFactor();
        brush->Release();
        maskBrush->Release();
        maskLayer->Release();
        bgImg->Release();
        render->Release();
        render = nullptr;
    }
}