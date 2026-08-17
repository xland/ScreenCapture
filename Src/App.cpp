#include "pch.h"
#include "App.h"
#include "Setting.h"
#include "Tray.h"
#include "Lang.h"
#include "Update.h"
#include "./Win/WinCap.h"
#include "./Win/WinPin.h"
#include "./Win/WinSetting.h"

std::unique_ptr<App> app;


App::~App()
{
}

void App::init()
{
    auto ptr = new App();
    app.reset(ptr);
}

void App::dispose()
{
    // 窗口对象是文件级静态变量，交给静态析构就晚了（那时 CoUninitialize 已经跑完），
    // 所以趁这里把还开着的窗口先放掉
    WinPin::dispose();
    WinCap::dispose();
    WinSetting::dispose();
    Lang::dispose();
    Setting::dispose();
    app.reset();
}

App* App::get()
{
    return app.get();
}

void App::takeScreenShot(int x, int y, int w, int h, ID2D1Bitmap1** img)
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    auto oldObj = SelectObject(hDC, hBitmap);
    BOOL bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    ReleaseDC(NULL, hScreen);
    std::vector<BYTE> data(w * 4 * h);
    BITMAPINFO bmi{};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = w;
    bmi.bmiHeader.biHeight = -h;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;
    GetDIBits(hDC, hBitmap, 0, h, data.data(), &bmi, DIB_RGB_COLORS);
    SelectObject(hDC, oldObj);
    DeleteDC(hDC);
    DeleteObject(hBitmap);
    D2D1_BITMAP_PROPERTIES1 props = {
       .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)},
       .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_NONE}
    };
    auto d2d = Ling::D2D::get();
    auto hr = d2d->deviceContext->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, props, img);
}

std::tuple<int, int, int, int> App::getScreenArea()
{
	return std::make_tuple(GetSystemMetrics(SM_XVIRTUALSCREEN), 
        GetSystemMetrics(SM_YVIRTUALSCREEN), 
        GetSystemMetrics(SM_CXVIRTUALSCREEN), 
        GetSystemMetrics(SM_CYVIRTUALSCREEN));
}

void App::excludeFromCapture(HWND hwnd)
{
    if (!hwnd) return;
    // 老系统上这个调用不但不失败，还会把窗口变成捕获画面里的一整块黑（实测 build 18363：
    // 返回 TRUE，读回来的 affinity 就是 0x11 —— 内核照存，可那会儿的 DWM 只认"非零即
    // 受保护内容"，一律涂黑）。所以必须自己拦住，让老系统退回"照旧被录进去"。
    // GetVersionEx 会被兼容性清单骗，只有 RtlGetVersion 给的是真版本号
    static const bool supported = []() {
        OSVERSIONINFOW vi{ sizeof(vi) };
        auto rtlGetVersion = (LONG(WINAPI*)(OSVERSIONINFOW*))GetProcAddress(
            GetModuleHandleW(L"ntdll.dll"), "RtlGetVersion");
        return rtlGetVersion && rtlGetVersion(&vi) == 0 && vi.dwBuildNumber >= 19041;
    }();
    if (!supported) return;
    SetWindowDisplayAffinity(hwnd, WDA_EXCLUDEFROMCAPTURE);
}

App::App()
{
    Ling::init();
    auto app = Ling::App::get();
    app->initArgs();
    Ling::D2D::addFonts({ L"icon.ttf" });
    // 录制中直接退出会让编码线程和 D3D 设备一起卡住，退出前先把录制停掉
    app->onBeforeQuit.add([]() { WinCap::stopIfRecording(); });
    Setting::init();
    Lang::init();
    if (app->args[L"--auto-quit"] == L"true") {
        WinCap::init();
    }
    else {
        bool flag = app->refuseSecondInstance();
        if (flag) return;
        Tray::init();
		// 开机自启不启动截图；--enter=tray 也一样，升级完重启新版本走的就是它 ——
		// 都是"只挂个托盘图标待命"，这条路上一个窗口都不建，图形设备也就根本不会创建
		if (app->args[L"--auto-start"] == L"true" || app->args[L"--enter"] == L"tray") {
			Update::checkLater();
			return;
		}
		WinCap::init();//默认情况下，应用启动随即进入截图模式
    }
}
