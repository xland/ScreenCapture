#include "pch.h"
#include "App.h"
#include "Win/WinCap.h"
#include "Win/WinLong.h"
#include "Win/WinVideo.h"
#include "Win/WinSetting.h"
#include "Win/WinPin.h"
#include "Setting.h"
#include "Lang.h"
#include "Tray.h"

// 各 Win* 类中的模块级单例/容器；App::disposeDeviceIfIdle 需要用来判断"当前有没有窗口"，
// 因此在这里 extern 引用。
extern std::unique_ptr<WinCap> winCap;
extern std::unique_ptr<WinLong> winLong;
extern std::unique_ptr<WinVideo> winVideo;
extern std::unique_ptr<WinSetting> winSetting;
extern std::vector<std::unique_ptr<WinPin>> winPins;

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
        {L"lang",L"false"},
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
    // 注意: 不在启动阶段建立 D2D/D3D/DXGI 设备了 —— 驻留态没有窗口时它们只占内存不干活。
    // 每个可见窗口在 init 时会调用 App::initDevice() 惰性建立；窗口全关后由
    // App::disposeDeviceIfIdle() 释放，回到驻留态时不再持有显卡驱动的工作集。
	app = std::make_unique<App>(hInstance);
    Setting::init();
    Lang::init();
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
    //正在录制GIF/MP4时，先停止录制并join编码线程，避免线程与MF/GDI资源卡死导致进程退不出去
    WinVideo::stopIfRecording();
    Setting::dispose();
    Lang::dispose();
    // 显式释放显卡/字体设备。进程退出时全局 ComPtr 也会析构，但那时 CRT 全局析构顺序不稳,
    // 提前 Reset 更保险。
    disposeDevice();
    PostQuitMessage(code);
}

void App::initDevice()
{
    // 幂等：设备已建立就直接返回，避免每次开截图/录屏都重新初始化。
    if (d2dFactory) return;
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

void App::disposeDevice()
{
    // 释放顺序：先高层的 device/factory, 后 D3D 设备。ComPtr::Reset 只是引用计数减一,
    // 若外部还持有对应对象则实际释放会延后到最后一个引用释放。
    dwriteFactory.Reset();
    d2dDev.Reset();
    fac5.Reset();
    d3d.Reset();
    d2dFactory.Reset();
}

void App::recreateDevice()
{
    disposeDevice();
    initDevice();
}

void App::disposeDeviceIfIdle()
{
    // 只有当所有 Win* 全部关闭时才真正回收设备; WinPin 是持久贴图窗口, 只要还留在屏幕
    // 就不能释放, 否则 WinPin 的 render 会随之失效。
    if (winCap) return;
    if (winLong) return;
    if (winVideo) return;
    if (winSetting) return;
    if (!winPins.empty()) return;
    disposeDevice();
}

void App::makeDC(WinBase* win)
{
    ComPtr<IDXGIDevice1> dxgiDev;
    d3d.As(&dxgiDev);
    auto hr = d2dDev->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, win->render.GetAddressOf());
    if (FAILED(hr)) { log(L"err:: makeDC CreateDeviceContext failed hr={:#x}", (unsigned)hr); return; }
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
    if (FAILED(hr)) { log(L"err:: makeDC CreateSwapChainForComposition failed hr={:#x}", (unsigned)hr); return; }
    hr = DCompositionCreateDevice(dxgiDev.Get(), IID_PPV_ARGS(win->compDev.GetAddressOf()));
    if (FAILED(hr)) { log(L"err:: makeDC DCompositionCreateDevice failed hr={:#x}", (unsigned)hr); return; }
    hr = win->compDev->CreateTargetForHwnd(win->hwnd, TRUE, win->compTgt.GetAddressOf());
    if (FAILED(hr)) { log(L"err:: makeDC CreateTargetForHwnd failed hr={:#x}", (unsigned)hr); return; }
    hr = win->compDev->CreateVisual(win->compVis.GetAddressOf());
    if (FAILED(hr)) { log(L"err:: makeDC CreateVisual failed hr={:#x}", (unsigned)hr); return; }
    hr = win->compVis->SetContent(win->swap.Get());
    if (FAILED(hr)) { log(L"err:: makeDC SetContent failed hr={:#x}", (unsigned)hr); return; }
    hr = win->compTgt->SetRoot(win->compVis.Get());
    if (FAILED(hr)) { log(L"err:: makeDC SetRoot failed hr={:#x}", (unsigned)hr); return; }
    hr = win->compDev->Commit();
    if (FAILED(hr)) { log(L"err:: makeDC Commit failed hr={:#x}", (unsigned)hr); return; }
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


