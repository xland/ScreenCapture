#include "pch.h"
#include <dwmapi.h>
#include <include/Ling.h>
#include <Windows.UI.Composition.Interop.h> 
#include "WinCap.h"
using namespace Microsoft::WRL;

namespace
{
    constexpr float scaleNum{ 5.f }, strokeWidth{ 2.f }, srcW{ 50.f }, srcH{ 30.f };
    constexpr float pixImgH{ scaleNum * srcH };
    constexpr float pixW{ srcW * scaleNum };
}

std::unique_ptr<WinCap> winCap;

WinCap::WinCap() : Ling::WinBase()
{
	setTitle(L"Screen Capture");
	onMouseDown.add([this](POINT pos, bool isRight) {
		if (isRight) {
			close();
            winCap.reset();
        }
        else {
			isPress = true;
			pressPos = pos;
        }
	});
	onMouseMove.add([this](POINT pos) {
        if (isPress) {
			dragMaskRect(pos);
        }
        else {
			switchWinRect(pos);
			getPixImg(pos);
			setPixPos(pos);
            refresh();
        }
	});
	onKeyDown.add([this](UINT key) { this->onKey(key); });
	onMouseUp.add([this](POINT pos, bool isRight) { isPress = false; });
}

WinCap::~WinCap()
{
}

void WinCap::init()
{
    auto ptr = new WinCap();
    winCap.reset(ptr);
    ptr->initPosSize();
	ptr->initWinRects();
    ptr->createNativeWindow(WS_EX_TOOLWINDOW, WS_POPUP);//WS_EX_TOPMOST
}

void WinCap::initPosSize()
{
	x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void WinCap::onCreated()
{
	takeScreenShot();
	auto d2d = Ling::D2D::get();
    surface = d2d->createDrawingSurface(compositor, (float)w, (float)h);
    auto brush = compositor.CreateSurfaceBrush(surface);
    brush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
    body->visual.Brush(brush);
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White), brushText.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x000000, 0.56f), brushBg.GetAddressOf());
    d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0x1677ff), brushBorder.GetAddressOf());
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
    D2D1_RECT_F destRect = D2D1::RectF(0, 0, (float)w, (float)h);
    if (screenImg) ctx->DrawBitmap(screenImg.Get(), destRect);
	paintMaskRect(ctx.Get()); 
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
    // 不夹紧 —— 夹紧虽然能保住尺寸，但中心就不再是光标像素，准星和取色都会跟光标错位。
    // 改成求交：越界的部分不画，交集部分按原位置摆进取景框，于是 (0,0) 处只有右下角有图。
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

void WinCap::dragMaskRect(POINT pos)
{
    if (pressPos.x < pos.x) {
        maskRect.left = pressPos.x;
        maskRect.right = pos.x;
    }
    else {
        maskRect.left = pos.x;
        maskRect.right = pressPos.x;
    }
    if (pressPos.y < pos.y) {
        maskRect.top = pressPos.y;
        maskRect.bottom = pos.y;
    }
    else {
        maskRect.top = pos.y;
        maskRect.bottom = pressPos.y;
    }
    refresh();
}

void WinCap::switchWinRect(POINT pos)
{
    for (auto& rect : winRect) {
        if (pos.x < rect.left || pos.x > rect.right || pos.y < rect.top || pos.y > rect.bottom) continue;
        if (maskRect.left == rect.left && maskRect.top == rect.top && maskRect.right == rect.right && maskRect.bottom == rect.bottom) break;
        maskRect = rect;
        refresh();
        break;
    }
}

void WinCap::takeScreenShot()
{
    HDC hScreen = GetDC(NULL);
    HDC hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    auto oldObj = SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, w, h, hScreen, x, y, SRCCOPY);
    ReleaseDC(NULL, hScreen);
    std::vector<BYTE> data(size_t(w) * 4 * size_t(h));
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
    // BitBlt 写出的是 BGRX —— 第 4 个字节是 0，不是 255。若按 PREMULTIPLIED 解读， 每个像素都是 alpha=0 的预乘色，整张图全透明。源位图用 IGNORE 让 D2D 把 alpha 当 1。
    // bitmapOptions 保持 NONE：这张图只作为 DrawBitmap 的源。
    D2D1_BITMAP_PROPERTIES1 props = {
       .pixelFormat{D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE)},
       .dpiX{96.0f}, .dpiY{96.0f}, .bitmapOptions{D2D1_BITMAP_OPTIONS_NONE}
    };
    auto d2d = Ling::D2D::get();
    auto hr = d2d->deviceContext->CreateBitmap(D2D1::SizeU(w, h), data.data(), w * 4, props, &screenImg);
}

void WinCap::paintMaskRect(ID2D1DeviceContext* ctx)
{
    ctx->FillRectangle(D2D1::RectF(0.f, 0.f, w, maskRect.top), brushBg.Get());
    ctx->FillRectangle(D2D1::RectF(0.f, maskRect.bottom, w, h), brushBg.Get());
    ctx->FillRectangle(D2D1::RectF(0.f, maskRect.top, maskRect.left, maskRect.bottom), brushBg.Get());
    ctx->FillRectangle(D2D1::RectF(maskRect.right, maskRect.top, w, maskRect.bottom), brushBg.Get());
	auto strokeWidth{ ::strokeWidth * dpi };
    auto halfStrokeWidth { strokeWidth / 2.f};
    ctx->DrawRectangle(D2D1::RectF(maskRect.left - halfStrokeWidth, maskRect.top - halfStrokeWidth, maskRect.right + halfStrokeWidth, maskRect.bottom + halfStrokeWidth), brushBorder.Get(), strokeWidth);
    auto str = std::format(L"X:{} Y:{} R:{} B:{} W:{} H:{}", maskRect.left, maskRect.top, maskRect.right, maskRect.bottom, maskRect.right - maskRect.left, maskRect.bottom - maskRect.top);
    auto d2d = Ling::D2D::get();
    Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
    d2d->dwriteFactory->CreateTextLayout(str.data(), (UINT32)str.length(), d2d->baseTextFormat.Get(), FLT_MAX, FLT_MAX, &textLayout);
    textLayout->SetFontSize(10.f*dpi, { 0,INT_MAX });
    DWRITE_TEXT_METRICS tm = {};
    textLayout->GetMetrics(&tm);
    float paddingLeft{ 5 * dpi }, padding{ 2 * 2 * dpi };
    auto layoutRect = D2D1::RectF(maskRect.left - strokeWidth, maskRect.top - tm.height - padding - strokeWidth, maskRect.left + tm.width + paddingLeft + strokeWidth * 2, maskRect.top - strokeWidth);
    if (layoutRect.top < 0) {
        layoutRect.top += tm.height + padding;
        layoutRect.bottom += tm.height + padding;
        layoutRect.left += strokeWidth;
    }
    ctx->FillRectangle(layoutRect, brushBg.Get());
    ctx->DrawTextLayout({ layoutRect.left + paddingLeft, layoutRect.top + padding / 2 }, textLayout.Get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
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
        auto d2d = Ling::D2D::get();
        Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
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
    bool flag{ false };
    if (key == VK_ESCAPE) {
        flag = true;
    }
    else if (key == 'H' && (GetKeyState(VK_CONTROL) & 0x8000)) {
		auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        wchar_t hex[8];
        swprintf_s(hex, L"#%02X%02X%02X", r, g, b);
        Ling::Util::setTextToClipboard(hex);
        flag = true;
    }
    else if (key == 'R' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        Ling::Util::setTextToClipboard(std::format(L"rgb({},{},{})", r, g, b));
        flag = true;
    }
    else if (key == 'K' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        auto cr = func();
        BYTE r = GetRValue(cr), g = GetGValue(cr), b = GetBValue(cr);
        auto [c, m, y1, k] = getCMYK(r, g, b);
        Ling::Util::setTextToClipboard(std::format(L"cmyk({},{},{},{})", c, m, y1, k));
        flag = true;
    }
    else if (key == 'P' && (GetKeyState(VK_CONTROL) & 0x8000)) {
        POINT pos;
        GetCursorPos(&pos);
        Ling::Util::setTextToClipboard(std::format(L"{},{}", pos.x, pos.y));
        flag = true;
    }
    if (flag) {
        close();
        winCap.reset();
    }
}

void WinCap::initWinRects()
{
    winRect.clear();
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) return TRUE;
            auto self = (WinCap*)lparam;
            if (rect.left < self->x) rect.left = self->x;
            if (rect.top < self->y) rect.top = self->y;
            if (rect.right > self->x + self->w) rect.right = self->x + self->w;
            if (rect.bottom > self->y + self->h) rect.bottom = self->y + self->h;
            auto x = (rect.left - self->x);
            auto y = (rect.top - self->y);
            auto r = (rect.right - self->x);
            auto b = (rect.bottom - self->y);
            self->winRect.push_back(D2D1::RectF(x, y, r, b));
            return TRUE;
        }, (LPARAM)this);
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