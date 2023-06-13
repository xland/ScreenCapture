#include "base/command_line.h"
#include "nativeui/nativeui.h"
#include "nativeui/window.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR lpCmdLine, _In_ int nCmdShow)
{
    base::CommandLine::Init(0, nullptr);
    nu::Lifetime lifetime;
    nu::State state;
    nu::Window::Options winOption;
    winOption.frame = false;
    scoped_refptr<nu::Window> window(new nu::Window(winOption));

    nu::RectF rect(GetSystemMetrics(SM_XVIRTUALSCREEN),
        GetSystemMetrics(SM_YVIRTUALSCREEN),
        GetSystemMetrics(SM_CXVIRTUALSCREEN),
        GetSystemMetrics(SM_CYVIRTUALSCREEN));

    HWND hwnd = window->GetWindowHandle();
    float scaleFactor;
    int dpi = GetDpiForWindow(hwnd);
    switch (dpi) {
    case 96:
        scaleFactor = 1.0;
        break;
    case 120:
        scaleFactor = 1.25;
        break;
    case 144:
        scaleFactor = 1.5;
        break;
    case 168:
        scaleFactor = 1.75;
        break;
    case 192:
        scaleFactor = 2.0;
        break;
    default:
        scaleFactor = 1.0;
        break;
    }



    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, rect.width(), rect.height());
    BOOL result = DeleteObject(SelectObject(hDC, hBitmap));
    if (!result) {
        //spdlog::get("logger")->info("DeleteObject error");
    }
    result = BitBlt(hDC, 0, 0, rect.width(), rect.height(), hScreen, rect.x(), rect.y(), SRCCOPY);
    if (!result) {
        //spdlog::get("logger")->info("BitBlt error");
    }
    unsigned int dataSize = ((rect.width() * 32 + 31) / 32) * 4 * rect.height();
    std::vector<std::uint8_t> pixels;
    pixels.resize(dataSize);
    BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(rect.width()), static_cast<long>(0-rect.height()), 1, 32, BI_RGB, dataSize, 0, 0, 0, 0 };
    int r = GetDIBits(hDC, hBitmap, 0, rect.height(), &pixels[0], &Info, DIB_RGB_COLORS);
    if (r == 0) {
        //spdlog::get("logger")->info("GetDIBits error");
    }
    auto buffer = nu::Buffer::Wrap(pixels.data(), dataSize);
    auto img = new nu::Image(buffer, scaleFactor);


    window->SetContentView(new nu::Label("Hello world"));
    window->SetHasShadow(false);
    window->SetBounds(rect);
    window->Activate();
    window->on_close.Connect([](nu::Window*) {
        nu::MessageLoop::Quit();
    });

    nu::MessageLoop::Run();
    return 0;
}