#include "pch.h"
#include "App.h"
#include "Win/WinCap.h"
#include "Win/WinLong.h"
#include "Win/WinVideo.h"
#include "Win/WinSetting.h"
#include "Setting.h"
#include "Tray.h"

namespace {
    std::unique_ptr<App> app;
    static ComPtr<ID2D1Factory1> d2dFactory;
    static ComPtr<ID3D11Device> d3d;
    static ComPtr<ID2D1Device1> d2dDev;
    static ComPtr<IDXGIFactory5> fac5;
    static ComPtr<IDWriteFactory5> dwriteFactory;
    //static ComPtr<IDWriteRenderingParams> renderingParams;     
    std::unordered_map<std::wstring, std::wstring> args{ 
        {L"enter",L"cap"},
        {L"tray",L"true"},
        {L"auto-quit",L"false"},
    };
}
BOOL App::allowTearing = FALSE;

App::App(HINSTANCE hInstance) : hInstance(hInstance)
{
    SetCurrentProcessExplicitAppUserModelID(L"github.xland.ScreenCapture");
}

App::~App()
{

}

void App::init(HINSTANCE hInstance)
{
    App::initArgs();
    if (args[L"auto-quit"] != L"true" && Tray::secondIns()) return;
    App::initDevice();
	app = std::make_unique<App>(hInstance);
    Setting::init();
    Tray::init();
    if (args.contains(L"--auto-start")) return; //开机自启动，不执行任何逻辑
    if (args[L"enter"] == L"long") {
        WinLong::init();
    }
    else if (args[L"enter"] == L"video") {
        WinVideo::init();
    }
    else {
        WinCap::init();
    }
}

App* App::get()
{
    return app.get();
}
void App::initArgs()
{
    LPWSTR* argv;
    int argc;
    LPWSTR cmdLine = GetCommandLine();
    argv = CommandLineToArgvW(cmdLine, &argc);
    for (int i = 1; i < argc; ++i) {
        std::wstring arg{ argv[i] };
        auto index = arg.find(L"=");
        if (index != std::wstring::npos) {
            args[arg.substr(0, index)] = arg.substr(index + 1);
        }
        else {
            args.insert({ arg,L"true"});
        }
    }
    LocalFree(argv);
}
void App::exit(const int& code)
{
    Setting::dispose();
    PostQuitMessage(code);
}

void App::initDevice()
{
    D2D1_FACTORY_OPTIONS opt{};
#ifdef DEBUG
    opt.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif
    auto hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, opt, d2dFactory.GetAddressOf());
    hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT | D3D11_CREATE_DEVICE_SINGLETHREADED,
        nullptr, 0, D3D11_SDK_VERSION, d3d.GetAddressOf(), nullptr, nullptr);
    ComPtr<IDXGIDevice1> dxgiDev;
    d3d.As(&dxgiDev);
    ComPtr<ID2D1Device> d2d;
    hr = d2dFactory->CreateDevice(dxgiDev.Get(), d2d.GetAddressOf());
    d2d.As(&d2dDev);
    hr = CreateDXGIFactory2(0, IID_PPV_ARGS(&fac5));
    hr = fac5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &App::allowTearing, sizeof(App::allowTearing));//判断设备是否允许撕裂呈现
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<::IUnknown**>(dwriteFactory.GetAddressOf()));
    //dwriteFactory->CreateCustomRenderingParams( 1.0f, 0.0f, 1.0f, DWRITE_PIXEL_GEOMETRY_RGB, DWRITE_RENDERING_MODE_GDI_NATURAL, renderingParams.GetAddressOf());
}

void App::makeDC(WinBase* win)
{
    ComPtr<IDXGIDevice1> dxgiDev;
    d3d.As(&dxgiDev);
    auto hr = d2dDev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, win->render.GetAddressOf());
    if (FAILED(hr)) return;
    //render->SetTextRenderingParams(renderingParams.Get());
    DXGI_SWAP_CHAIN_DESC1 scd{};
    scd.Width = win->w;
    scd.Height = win->h;
    scd.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount = 2;
    scd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    scd.SampleDesc.Count = 1;
    scd.AlphaMode = DXGI_ALPHA_MODE_PREMULTIPLIED;
    if (App::allowTearing) scd.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    hr = fac5->CreateSwapChainForComposition(d3d.Get(), &scd, nullptr, win->swap.GetAddressOf());
    if (FAILED(hr)) return;
    hr = DCompositionCreateDevice(dxgiDev.Get(), IID_PPV_ARGS(win->compDev.GetAddressOf()));
    if (FAILED(hr)) return;
    hr = win->compDev->CreateTargetForHwnd(win->hwnd, TRUE, win->compTgt.GetAddressOf());
    if (FAILED(hr)) return;
    hr = win->compDev->CreateVisual(win->compVis.GetAddressOf());
    if (FAILED(hr)) return;
    hr = win->compVis->SetContent(win->swap.Get());
    if (FAILED(hr)) return;
    hr = win->compTgt->SetRoot(win->compVis.Get());
    if (FAILED(hr)) return;
    hr = win->compDev->Commit();
    if (FAILED(hr)) return;
}

std::wstring& App::getArg(const std::wstring& key)
{
    return args[key];
}

ID2D1Factory1* App::getD2D()
{
    return d2dFactory.Get();
}

IDWriteFactory5* App::getWriter()
{
    return dwriteFactory.Get();
}


