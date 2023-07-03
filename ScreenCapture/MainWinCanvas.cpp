#include "MainWin.h"



void MainWin::initCanvas()
{
    ComPtr<ID2D1Factory> d2dFactory;
    {
        //创建可用于创建 Direct2D 资源的工厂对象。
        D2D1_FACTORY_OPTIONS fo = {};
        #ifdef _DEBUG
            fo.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
        #endif
        HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, fo, d2dFactory.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDWriteFactory> dwrite;
    {
        //创建一个DirectWrite工厂对象，用于后续创建单个DirectWrite对象。
        //与绘制文字有关，暂时没用到
        HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, 
            __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(dwrite.GetAddressOf()));
        if (!SUCCEEDED(hr)) return;
    }    
    {
        //可以通过在 ID2D1GdiInteropRenderTarget 的呈现器目标上调用 QueryInterface，
        //在 Direct2D 呈现目标上使用 GDI 进行呈现，该呈现目标上具有可用于检索 GDI 设备上下文的 GetDC 和 ReleaseDC 方法。
        //仅当创建具有D2D1_RENDER_TARGET_USAGE_GDI_COMPATIBLE标志的呈现目标时，才能通过 GDI 进行呈现。 
        //这对于主要使用 Direct2D 呈现但具有扩展性模型或需要 GDI 呈现功能的其他旧内容的应用程序非常有用。
        HRESULT hr =d2dFactory->QueryInterface(d2dfactory1.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }    
    ComPtr<ID3D11Device> d3d11Device;
    {
        //创建表示显示适配器的设备。
        UINT flags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
        #ifdef _DEBUG
            flags |= D3D11_CREATE_DEVICE_DEBUG;
        #endif
        D3D_FEATURE_LEVEL featureLevels[] =
        {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };
        HRESULT hr = D3D11CreateDevice(nullptr,
            D3D_DRIVER_TYPE_HARDWARE, //todo D3D_DRIVER_TYPE_WARP
            nullptr, flags, featureLevels,
            sizeof(featureLevels) / sizeof(*featureLevels),
            D3D11_SDK_VERSION, d3d11Device.GetAddressOf(), nullptr, nullptr);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIDevice> dxgiDevice;
    {
        //IDXGIDevice 接口为生成图像数据的 DXGI 对象实现派生类。
        HRESULT hr = d3d11Device->QueryInterface(dxgiDevice.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<ID2D1Device> d2d1Device;
    {
        //表示一个资源域，其对象和设备上下文可以一起使用。
        HRESULT hr = d2dfactory1->CreateDevice(dxgiDevice.Get(), d2d1Device.GetAddressOf());
        if (!SUCCEEDED(hr)) return;
    }    
    {
        //表示用于呈现给目标的一组状态和命令缓冲区。
        //设备上下文可以呈现为目标位图或命令列表。
        HRESULT hr = d2d1Device->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIAdapter> dxgiAdapter;
    {
        //表示显示子系统， (包括一个或多个 GPU、DAC 和视频内存) 。
        HRESULT hr = dxgiDevice->GetAdapter(&dxgiAdapter);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGIFactory2> dxgiFactory;
    {
        //包括创建具有比IDXGISwapChain更多功能的新版本交换链和监控立体3D功能的方法。
        HRESULT hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory2), (void**)&dxgiFactory);
        if (!SUCCEEDED(hr)) return;
    }    
    {
        //提供从IDXGISwapChain增强的表示功能。这些表示功能包括指定脏矩形和滚动矩形以优化表示。
        DXGI_SWAP_CHAIN_DESC1 props = {};
        props.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
        props.SampleDesc.Count = 1;
        props.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        props.BufferCount = 2;
        props.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
        props.AlphaMode = DXGI_ALPHA_MODE_IGNORE;


        HRESULT hr = dxgiFactory->CreateSwapChainForHwnd(d3d11Device.Get(), hwnd, &props, nullptr, nullptr, &dxgiSwapChain);
        if (!SUCCEEDED(hr)) return;
    }
    ComPtr<IDXGISurface> dxgiSurface;
    {
        //idxgissurface接口实现了图像数据对象的方法。
        IDXGISurface* surface = nullptr;
        HRESULT hr = dxgiSwapChain->GetBuffer(0, __uuidof(IDXGISurface), (void**)&surface);
        if (!SUCCEEDED(hr))return;
        dxgiSurface = surface;
    } 
    
    {
        //从DXGI表面创建位图，可以将其设置为target surface或指定其他颜色上下文信息。
        auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE));
        HRESULT hr = context->CreateBitmapFromDxgiSurface(dxgiSurface.Get(), props, &d2dBitmap);
        if (!SUCCEEDED(hr)) return;
    }   

    
}

void MainWin::paintBg()
{
    static ComPtr<ID2D1Bitmap> bgImg;
    if (!bgImg.Get())
    {
        D2D1_BITMAP_PROPERTIES imgConfig;
        imgConfig.dpiX = 0.0f;
        imgConfig.dpiY = 0.0f;
        imgConfig.pixelFormat.format = DXGI_FORMAT_B8G8R8A8_UNORM;
        imgConfig.pixelFormat.alphaMode = D2D1_ALPHA_MODE_IGNORE;
        context->CreateBitmap(D2D1::SizeU(w, h), bgPixels, 4 * w, imgConfig, bgImg.GetAddressOf());       
        delete[] bgPixels;
    }    
    context->DrawBitmap(bgImg.Get(), D2D1::RectF(0, 0, w, h));
    

    //static ComPtr<ID2D1Layer> layer;
    //if (!layer.Get()) {
    //    context->CreateLayer(NULL, &layer);
    //}
    //static ComPtr<ID2D1SolidColorBrush> defaultBrush;
    //if (!defaultBrush.Get()) {
    //    context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &defaultBrush);
    //}
    //static ComPtr<ID2D1SolidColorBrush> clearBrush;
    //if (!clearBrush.Get()) {
    //    context->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 1), &clearBrush);
    //}
    //auto param = D2D1::LayerParameters(D2D1::InfiniteRect(), NULL, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
    //    D2D1::IdentityMatrix(), 1.0, clearBrush.Get(), D2D1_LAYER_OPTIONS_NONE);
    //context->PushLayer(param, layer.Get());
    //context->DrawLine(D2D1::Point2F(0.0f, 0.0f), D2D1::Point2F(w, h), defaultBrush.Get(), 16.5f);
    //context->DrawLine(D2D1::Point2F(0.0f, h), D2D1::Point2F(w, 0), defaultBrush.Get(),16.5f);
    //context->PopLayer();
}

void MainWin::paintCanvas()
{
    if (!canvasImg.Get())
    {
        unsigned int dataSize = w * h * 4;
        char* pixels = new char[dataSize];
        memset(pixels, 0, dataSize);
        auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
        context->CreateBitmap(D2D1::SizeU(w, h), pixels, 4 * w, props, &canvasImg);
        delete[] pixels;
    }
    static ComPtr<ID2D1SolidColorBrush> defaultBrush;
    if (!defaultBrush.Get()) {
        context->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Red), &defaultBrush);
    }
    context->SetTarget(canvasImg.Get());
    context->FillRectangle(D2D1::RectF(80.0f, 80.0f, 1280.0f, 1280.0f ), defaultBrush.Get());
    //defaultBrush->SetColor(D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f));
    //context->DrawLine(D2D1::Point2F(0.0f, h), D2D1::Point2F(w, 0), defaultBrush.Get(),16.5f);
    context->SetTarget(d2dBitmap.Get());
    context->DrawBitmap(canvasImg.Get(), D2D1::RectF(0, 0, w, h));
}

void MainWin::paint()
{
    context->SetTarget(d2dBitmap.Get());
    context->BeginDraw();
    context->SetTransform(D2D1::Matrix3x2F::Identity());
    paintBg();
    if (state != State::Eraser) {
        paintCanvas();
    }
    else {
        paintEraser();
    } 
    paintMask();
    context->EndDraw();
    context->Flush();
    DXGI_PRESENT_PARAMETERS parameters = { 0 };
    dxgiSwapChain->Present1(1, 0, &parameters);
}

void MainWin::paintMask()
{
    static ComPtr<ID2D1Layer> layer;
    if (!layer.Get()) {
        context->CreateLayer(NULL, &layer);
    }
    static ComPtr<ID2D1SolidColorBrush> bgBrush;
    if (!bgBrush.Get()) {
        context->CreateSolidColorBrush(D2D1::ColorF(0, 0, 0, 0.68f), &bgBrush);
    }
    static ComPtr<ID2D1SolidColorBrush> borderBrush;
    if (!borderBrush.Get()) {
        context->CreateSolidColorBrush(D2D1::ColorF(0x9ACD32, 1.0f), &borderBrush);
    }
    auto param = D2D1::LayerParameters1(D2D1::InfiniteRect(),NULL,D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
        D2D1::IdentityMatrix(),1,NULL, D2D1_LAYER_OPTIONS1_NONE);
    context->PushLayer(param, layer.Get());
    context->FillRectangle(D2D1::RectF(0, 0, w, h), bgBrush.Get());
    context->DrawRectangle(cutRect, borderBrush.Get(),8.0f);
    context->PushAxisAlignedClip(cutRect, D2D1_ANTIALIAS_MODE_ALIASED);
    context->Clear(D2D1::ColorF(0, 0, 0, 0));
    context->PopAxisAlignedClip();
    context->PopLayer();
}

void MainWin::paintEraser()
{
    //D2D1_BITMAP_BRUSH_PROPERTIES1 propertiesXClampYClamp = D2D1::BitmapBrushProperties1(D2D1_EXTEND_MODE_CLAMP,
    //                D2D1_EXTEND_MODE_CLAMP,D2D1_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
    //ComPtr<ID2D1BitmapBrush1> bitmapBrush;
    //context->CreateBitmapBrush(canvasImg.Get(), propertiesXClampYClamp, &bitmapBrush);
    //context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
    //context->FillOpacityMask(eraserImg.Get(),bitmapBrush.Get(),
    //    D2D1_OPACITY_MASK_CONTENT_GRAPHICS);
    //context->SetAntialiasMode(D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
        
    static ComPtr<ID2D1Effect> compositeEffect;
    if (!compositeEffect.Get()) {
        context->CreateEffect(CLSID_D2D1Composite, &compositeEffect);
    }
    compositeEffect->SetInput(0, eraserImg.Get());
    context->SetTarget(canvasImg.Get());
    context->DrawImage(compositeEffect.Get(), D2D1_INTERPOLATION_MODE_LINEAR,D2D1_COMPOSITE_MODE_SOURCE_OUT);
    context->SetTarget(d2dBitmap.Get());
    context->DrawBitmap(canvasImg.Get(), D2D1::RectF(0, 0, w, h));
}