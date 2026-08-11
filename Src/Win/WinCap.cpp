#include "pch.h"
#include <include/Ling.h>
#include <Windows.UI.Composition.Interop.h>
#include "WinCap.h"
#include "WinPin.h"
#include "CutMask.h"
#include "../App.h"
using namespace Microsoft::WRL;

namespace
{
    constexpr float scaleNum{ 5.f }, srcW{ 50.f }, srcH{ 30.f };
    constexpr float pixImgH{ scaleNum * srcH };
    constexpr float pixW{ srcW * scaleNum };
}

std::unique_ptr<WinCap> winCap;

WinCap::WinCap() : Ling::WinBase()
{
	setTitle(L"Screen Capture");
    auto [x1, y1, w1, h1] = App::get()->getScreenArea();
	this->x = x1;this->y = y1;this->w = w1;this->h = h1;
	onMouseDown.add([this](POINT pos, bool isRight) {
		if (isRight) { close(); }
        else {
			isPress = true;
			cutMask->startMakeRect(pos);
        }
	});
	onMouseMove.add([this](POINT pos) {
        if (isPress) {
			cutMask->makeRect(pos);
        }
        else {
			cutMask->highlight(pos);
			getPixImg(pos);
			setPixPos(pos);
            refresh();
        }
	});
	onKeyDown.add([this](UINT key) { this->onKey(key); });
	onMouseUp.add([this](POINT pos, bool isRight) {
        isPress = false;
        auto& maskRect = cutMask->maskRect;
        WinPin::init(int(maskRect.left) + x, int(maskRect.top) + y, int(maskRect.right - maskRect.left), int(maskRect.bottom - maskRect.top));
        close();
    });
	onDestroy.add([this]() {
        winCap.reset();
    });
}

WinCap::~WinCap()
{
}

void WinCap::init()
{
    auto ptr = new WinCap();
    winCap.reset(ptr);
	ptr->cutMask = std::make_unique<CutMask>(ptr);
    ptr->createNativeWindow(WS_EX_TOOLWINDOW, WS_POPUP);//WS_EX_TOPMOST
}

WinCap* WinCap::get()
{
    return winCap.get();
}

void WinCap::onCreated()
{
    App::get()->takeScreenShot(x, y, w, h, &screenImg);
	auto d2d = Ling::D2D::get();
    surface = d2d->createDrawingSurface(compositor, (float)w, (float)h);
    auto brush = compositor.CreateSurfaceBrush(surface);
    brush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
    body->visual.Brush(brush);
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.56f), brushBg.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0.1f, 0.5f, 1.f, 0.5f), crossBrush.GetAddressOf());
    POINT pos;
    GetCursorPos(&pos);
    ScreenToClient(hwnd, &pos);
    getPixImg(pos);
    setPixPos(pos);
    show();
}

void WinCap::layout()
{
    Ling::WinBase::layout();
    auto s = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
    ComPtr<ID2D1DeviceContext> ctx;
    POINT offset{};
    s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), reinterpret_cast<void**>(ctx.GetAddressOf()), &offset);
    auto trans = D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y);
    ctx->SetTransform(trans);
    ctx->Clear(0);
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, (float)w, (float)h);
    ctx->DrawBitmap(screenImg.Get(), destRect);
	cutMask->paint(ctx.Get());
	paintPix(ctx.Get());
    s->EndDraw();
}

BOOL WinCap::setCursor()
{
    SetCursor(LoadCursor(nullptr, IDC_CROSS));
    return TRUE;
}

void WinCap::setPixPos(POINT pos)
{
    auto span{ 10 * dpi };
    pixPos.x = int(pos.x + span + dpi);
    if (pixPos.x + pixW > w) {
        pixPos.x = int(pos.x - span - pixW + dpi);
    }
    pixPos.y = int(pos.y + span + dpi);
    auto pixH{ pixImgH + 76.f * dpi };
    if (pixPos.y + pixH > h) {
        pixPos.y = int(pos.y - span - pixH + dpi);
    }
}

void WinCap::getPixImg(POINT pos)
{
    if (isPress) return;
    const long sw = static_cast<long>(srcW), sh = static_cast<long>(srcH);
    const long iw = static_cast<long>(w), ih = static_cast<long>(h);
    // 期望的源矩形：以光标为正中心，可以越出屏幕。
    const long wantL = pos.x - sw / 2, wantT = pos.y - sh / 2;
    long vl = std::max(wantL, 0L), vt = std::max(wantT, 0L);
    long vr = std::min(wantL + sw, iw), vb = std::min(wantT + sh, ih);
    // 交集在 pixImg 内的落点：期望矩形左上角为原点，所以减 wantL/wantT（越界时为正偏移）
    const long dl = vl - wantL, dt = vt - wantT;
    if (!pixImg) {
        auto d2d = Ling::D2D::get();
        D2D1_BITMAP_PROPERTIES1 prop{};
        prop.pixelFormat = screenImg->GetPixelFormat();
        prop.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
        screenImg->GetDpi(&prop.dpiX, &prop.dpiY);
        auto hr = d2d->deviceContext->CreateBitmap(D2D1::SizeU(sw, sh), nullptr, 0, &prop, &pixImg);
    }
    auto srcRect = D2D1::RectU(vl, vt, vr, vb);
    auto destPoint = D2D1::Point2U(static_cast<UINT32>(dl), static_cast<UINT32>(dt));
    auto hr = pixImg->CopyFromBitmap(&destPoint, screenImg.Get(), &srcRect);
    // pixImg 里这一帧真正有内容的区域。越界部分留着上一帧的残留，
    // 绘制时靠 DrawBitmap 的 srcRect 把它排除掉，露出取景框自己的底色。
    pixSrcRect = D2D1::RectF((float)dl, (float)dt, float(dl + (vr - vl)), float(dt + (vb - vt)));
}

void WinCap::paintPix(ID2D1DeviceContext* ctx)
{
	if (isPress) return;
    D2D1_RECT_F pixRect{ (float)pixPos.x, (float)pixPos.y, pixPos.x + pixW, pixPos.y + pixImgH + 76.f * dpi };
    ctx->FillRectangle(pixRect, brushBg.Get());
    if (pixSrcRect.right > pixSrcRect.left && pixSrcRect.bottom > pixSrcRect.top) {
        D2D1_RECT_F imgRect{
            pixPos.x + pixSrcRect.left * scaleNum,
            pixPos.y + pixSrcRect.top * scaleNum,
            pixPos.x + pixSrcRect.right * scaleNum,
            pixPos.y + pixSrcRect.bottom * scaleNum
        };
        ctx->DrawBitmap(pixImg.Get(), imgRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR, &pixSrcRect);
    }
    ctx->DrawRectangle(pixRect, brushBg.Get(),dpi);

    float crossWHalf{ 4.f*dpi };
    auto crossRect0 = D2D1::RectF(pixPos.x, pixPos.y+pixImgH / 2 - crossWHalf, pixPos.x+pixW / 2 - crossWHalf, pixPos.y + pixImgH / 2 + crossWHalf);
    auto crossRect1 = D2D1::RectF(pixPos.x + pixW / 2 + crossWHalf, pixPos.y+pixImgH / 2 - crossWHalf, pixPos.x + pixW, pixPos.y + pixImgH / 2 + crossWHalf);
    auto crossRect2 = D2D1::RectF(pixPos.x + pixW / 2 - crossWHalf, pixPos.y, pixPos.x + pixW / 2 + crossWHalf, pixPos.y + pixImgH / 2 - crossWHalf);
    auto crossRect3 = D2D1::RectF(pixPos.x + pixW / 2 - crossWHalf, pixPos.y+pixImgH / 2 + crossWHalf, pixPos.x + pixW / 2 + crossWHalf, pixPos.y + pixImgH);

    ctx->FillRectangle(crossRect0, crossBrush.Get());
    ctx->FillRectangle(crossRect1, crossBrush.Get());
    ctx->FillRectangle(crossRect2, crossBrush.Get());
    ctx->FillRectangle(crossRect3, crossBrush.Get());

    POINT pos;
	GetCursorPos(&pos);
    HDC hScreen = GetDC(NULL);
    COLORREF cr = GetPixel(hScreen, pos.x, pos.y);
    ReleaseDC(NULL, hScreen);
    auto d2d = Ling::D2D::get();
    float padding{ 7.f * dpi },fontSize{ 10.f*dpi };
    Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
    for (size_t i = 0; i < 4; i++)
    {
        std::wstring str;
        if (i == 0) {
            str = std::format(L"HEX (Ctrl+H) : #{:02X}{:02X}{:02X}", GetRValue(cr), GetGValue(cr), GetBValue(cr));
		}
		else if (i == 1) {
			str = std::format(L"RGB (Ctrl+R) : {},{},{}", GetRValue(cr), GetGValue(cr), GetBValue(cr));
		}
        else if (i == 2) {
            auto [c, m, y1, k] = getCMYK(GetRValue(cr), GetGValue(cr), GetBValue(cr));
            str = std::format(L"CMYK (Ctrl+K) : {},{},{},{}", c, m, y1,k);
        }
        else if (i == 3) {
            str = std::format(L"POS (Ctrl+P) : X:{} Y:{}", pos.x, pos.y);
        }
        d2d->dwriteFactory->CreateTextLayout(str.data(), (UINT32)str.length(), d2d->baseTextFormat.Get(), FLT_MAX, FLT_MAX, &textLayout);
        textLayout->SetFontSize(fontSize, { 0,INT_MAX });
        ctx->DrawTextLayout({ pixPos.x + padding, pixPos.y + pixImgH + padding*(i+1) + fontSize*i }, textLayout.Get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
    }
}

void WinCap::onKey(UINT key)
{
    auto func = []() {
        POINT pos;
        GetCursorPos(&pos);
        HDC hScreen = GetDC(NULL);
        COLORREF cr = GetPixel(hScreen, pos.x, pos.y);
        ReleaseDC(NULL, hScreen);
        return cr;
    };
    if (key == VK_ESCAPE) {
        close();
    }
    else if (key == 'H' && (GetKeyState(VK_CONTROL) & 0x8000)) {
		auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        wchar_t hex[8];
        swprintf_s(hex, L"#%02X%02X%02X", r, g, b);
        Ling::Util::setTextToClipboard(hex);
        close();
    }
    else if (key == 'R' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        Ling::Util::setTextToClipboard(std::format(L"rgb({},{},{})", r, g, b));
        close();
    }
    else if (key == 'K' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        auto [c, m, y1, k] = getCMYK(r, g, b);
        Ling::Util::setTextToClipboard(std::format(L"cmyk({},{},{},{})", c, m, y1, k));
        close();
    }
    else if (key == 'P' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        POINT pos;
        GetCursorPos(&pos);
        Ling::Util::setTextToClipboard(std::format(L"{},{}", pos.x, pos.y));
        close();
    }
}

std::tuple<int, int, int, int> WinCap::getCMYK(const BYTE& r, const BYTE& g, const BYTE& b)
{
    double R = r / 255.0, G = g / 255.0, B = b / 255.0;
    double K = 1.0 - (std::max)(R, (std::max)(G, B));
    double C = (K == 1.0) ? 0.0 : (1.0 - R - K) / (1.0 - K);
    double M = (K == 1.0) ? 0.0 : (1.0 - G - K) / (1.0 - K);
    double Y = (K == 1.0) ? 0.0 : (1.0 - B - K) / (1.0 - K);
    return std::make_tuple(static_cast<int>(std::round(C * 100)),
        static_cast<int>(std::round(M * 100)),
        static_cast<int>(std::round(Y * 100)),
        static_cast<int>(std::round(K * 100))
    );
}
ComPtr<ID2D1Bitmap1> WinCap::getCutImg()
{
    ComPtr<ID2D1Bitmap1> cutImg;
    auto& maskRect = cutMask->maskRect;
    const UINT32 cw = (UINT32)(maskRect.right - maskRect.left);
    const UINT32 ch = (UINT32)(maskRect.bottom - maskRect.top);
    if (cw == 0 || ch == 0) return cutImg;
    D2D1_BITMAP_PROPERTIES1 prop{};
    prop.pixelFormat = screenImg->GetPixelFormat();
    prop.bitmapOptions = D2D1_BITMAP_OPTIONS_NONE;
    screenImg->GetDpi(&prop.dpiX, &prop.dpiY);
    Ling::D2D::get()->deviceContext->CreateBitmap(D2D1::SizeU(cw, ch), nullptr, 0, &prop, cutImg.GetAddressOf());
    auto start = D2D1::Point2U(0, 0);
    auto rect = D2D1::RectU((UINT32)maskRect.left, (UINT32)maskRect.top, (UINT32)maskRect.right, (UINT32)maskRect.bottom);
    cutImg->CopyFromBitmap(&start, screenImg.Get(), &rect);
    return cutImg;
}