#include "pch.h"
#include <Windows.UI.Composition.Interop.h> 
#include <TinyOCR/include/tinyocr/tiny_ocr.h>
#include <wincodec.h>
#include "ImgViewer.h"
#include "WindowMain.h"
#include "TextBox.h"

using namespace Microsoft::WRL;


ImgViewer::ImgViewer(Ling::WinBase* win) :Ling::Node(win)
{
	setHeightPercent(100.f);
	setFlexGrow(1.f);
	win->onMouseDown.add([this](POINT pt, bool isRight) {this->onDown(pt, isRight);});
	win->onMouseMove.add([this](POINT pt) {this->onMove(pt);});
	win->onMouseUp.add([this](POINT pt, bool isRight) {this->onUp(pt);});
}

ImgViewer::~ImgViewer()
{
}

void ImgViewer::loadImg(const std::wstring& imgPath)
{
	ComPtr<IWICImagingFactory> wicFactory;
	auto hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
	ComPtr<IWICBitmapDecoder> decoder;
	hr = wicFactory->CreateDecoderFromFilename(imgPath.data(), nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
	ComPtr<IWICBitmapFrameDecode> frame = nullptr;
	hr = decoder->GetFrame(0, &frame);
	ComPtr<IWICFormatConverter> converter = nullptr;
	hr = wicFactory->CreateFormatConverter(&converter);
	hr = converter->Initialize(frame.Get(), GUID_WICPixelFormat32bppPBGRA, WICBitmapDitherTypeNone, NULL, 0.f, WICBitmapPaletteTypeMedianCut);

	auto d2d = Ling::D2D::get();
	hr = d2d->deviceContext->CreateBitmapFromWicBitmap(converter.Get(), nullptr, &bitmap);

	UINT w = 0, h = 0;
	converter->GetSize(&w, &h);
	std::vector<uint8_t> bgra(static_cast<size_t>(w) * h * 4);
	converter->CopyPixels(nullptr, w * 4, static_cast<UINT>(bgra.size()), bgra.data());

	readImg(bgra.data(), w, h);
	win->refresh();
}

void ImgViewer::paint()
{
	const int pxW = static_cast<int>(w);
	const int pxH = static_cast<int>(h);
	if (pxW <= 0 || pxH <= 0 || !bitmap) return;
	if (!surface) {
		auto d2d = Ling::D2D::get();
		surface = d2d->createDrawingSurface(win->compositor, (float)pxW, (float)pxH);
		auto surfaceBrush = win->compositor.CreateSurfaceBrush(surface);
		surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
		visual.Brush(surfaceBrush);
	}
	else {
		auto sz = surface.SizeInt32();
		if (sz.Width != pxW || sz.Height != pxH) {
			surface.Resize({ pxW, pxH });
		}
	}
	auto s = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
	ComPtr<ID2D1DeviceContext> ctx;
	POINT offset{};
	s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), reinterpret_cast<void**>(ctx.GetAddressOf()), &offset);
	ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));
	ctx->Clear(0);
	drawBitmap(ctx.Get());
	drawRects(ctx.Get(), offset);
	s->EndDraw();
}

void ImgViewer::getDrawParams(float& scale, float& dx, float& dy) const
{
	// 默认值：bitmap 异常时退化为不缩放、不偏移
	scale = 1.f;
	dx = 0.f;
	dy = 0.f;
	if (!bitmap) return;

	const D2D1_SIZE_F bmpSize = bitmap->GetSize();
	const float imgW = bmpSize.width;
	const float imgH = bmpSize.height;
	if (imgW <= 0.f || imgH <= 0.f) return;

	// 控件尺寸与位图都是物理像素（Yoga 已乘过 dpi，bitmap 也是物理像素），可直接比
	const float vw = w;
	const float vh = h;
	// contain: 取较小那一维的缩放系数，保证两边都塞得进
	scale = (vw / imgW < vh / imgH) ? (vw / imgW) : (vh / imgH);
	if (scale > 1.f) scale = 1.f;  // 不放大 —— 图比控件小时保持原尺寸

	const float drawW = imgW * scale;
	const float drawH = imgH * scale;
	dx = (vw - drawW) * 0.5f;
	dy = (vh - drawH) * 0.5f;
}

void ImgViewer::drawBitmap(ID2D1DeviceContext* ctx)
{
	if (!ctx || !bitmap) return;
	float scale, dx, dy;
	getDrawParams(scale, dx, dy);
	const D2D1_SIZE_F bmpSize = bitmap->GetSize();
	const float drawW = bmpSize.width * scale;
	const float drawH = bmpSize.height * scale;
	// 把位图直接 DrawBitmap 到 (dx, dy) ~ (dx+drawW, dy+drawH) —— 缩放由 D2D 内插完成
	D2D1_RECT_F dstRect{ dx, dy, dx + drawW, dy + drawH };
	ctx->DrawBitmap(bitmap.Get(), dstRect, 1.0f, D2D1_BITMAP_INTERPOLATION_MODE_LINEAR);
}

void ImgViewer::drawRects(ID2D1DeviceContext* ctx, POINT surfaceOffset)
{
	if (!ctx || !bitmap || !doc.hasSelection()) return;
	// 字符框是原图坐标系（与 bitmap 同尺度），要画到 surface 上需要套上和 drawBitmap 完全相同的「等比缩放 + 居中」变换；再加 BeginDraw 给的 surface tile offset。
	float scale, dx, dy;
	getDrawParams(scale, dx, dy);
	D2D1_MATRIX_3X2_F oldTransform;
	ctx->GetTransform(&oldTransform);
	// D2D 是行向量约定：A * B 表示「先 A 再 B」。原图坐标 -> 先等比缩放 -> 再平移居中 -> 最后叠加 surface tile offset
	auto imgTransform = D2D1::Matrix3x2F::Scale(scale, scale) * D2D1::Matrix3x2F::Translation(dx, dy);
	auto surfaceT = D2D1::Matrix3x2F::Translation((float)surfaceOffset.x, (float)surfaceOffset.y);
	ctx->SetTransform(imgTransform * surfaceT);
	// 懒创建半透明画刷（淡绿色）—— 设备相关资源，第一次 paint 时建一次即可
	if (!overlayBrush) {
		ctx->CreateSolidColorBrush(Ling::Color(0x389e0d66).getD2DColor(), overlayBrush.GetAddressOf());
	}
	// 只画选中区间内的字符框
	const auto& lines = doc.getLines();
	for (int li = 0; li < (int)lines.size(); ++li) {
		int b, e;
		if (!doc.getLineRange(li, b, e)) continue;
		const auto& geoms = lines[li].charGeoms;
		for (int i = b; i < e && i < (int)geoms.size(); ++i) {
			if (geoms[i]) ctx->FillGeometry(geoms[i].Get(), overlayBrush.Get());
		}
	}
	// 还原 transform，避免影响后续绘制
	ctx->SetTransform(oldTransform);
}

bool ImgViewer::toImagePos(POINT pt, float& px, float& py) const
{
	if (!bitmap) return false;
	float scale, dx, dy;
	getDrawParams(scale, dx, dy);
	if (scale <= 0.f) return false;
	// pt 是窗口坐标；先减去控件原点，再逆着 drawBitmap 的变换回到原图坐标
	px = ((float)pt.x - x - dx) / scale;
	py = ((float)pt.y - y - dy) / scale;
	return true;
}

bool ImgViewer::isPosInImage(POINT pt) const
{
	if (!bitmap) return false;
	float scale, dx, dy;
	getDrawParams(scale, dx, dy);
	const D2D1_SIZE_F sz = bitmap->GetSize();
	const float lx = x + dx, ty = y + dy;
	return pt.x >= lx && pt.x < lx + sz.width * scale
		&& pt.y >= ty && pt.y < ty + sz.height * scale;
}

void ImgViewer::onDown(POINT pt, bool isRight)
{
	if (isRight || doc.empty()) return;
	if (!isPosInImage(pt)) return;
	float px, py;
	if (!toImagePos(pt, px, py)) return;
	doc.setAnchor(doc.hitTestImage(px, py));
	selecting = true;
	SetCapture(win->hwnd);
	doc.notifyChanged();
}

void ImgViewer::onMove(POINT pt)
{
	if (!selecting) return;
	float px, py;
	if (!toImagePos(pt, px, py)) return;
	// 拖拽中不再要求落在图内 —— 越界时 hitTestImage 会吸附到首/尾，和浏览器选文本的手感一致
	auto pos = doc.hitTestImage(px, py);
	if (pos == doc.getFocus()) return;   // 没跨到新字符就不重画
	doc.setFocus(pos);
	doc.notifyChanged();
}

void ImgViewer::onUp(POINT pt)
{
	if (!selecting) return;
	selecting = false;
	ReleaseCapture();
}

void ImgViewer::layout()
{
	Node::layout();
	paint();
}

void ImgViewer::readImg(const uint8_t* data, UINT w, UINT h)
{
	std::vector<uint8_t> bgr(static_cast<size_t>(w) * h * 3);
	auto dst = bgr.data();
	const size_t n = static_cast<size_t>(w) * h;
	for (size_t i = 0; i < n; ++i) {
		dst[0] = data[0];
		dst[1] = data[1];
		dst[2] = data[2];
		data += 4;
		dst += 3;
	}
	auto [recData, recSize] = Ling::Util::getRes(L"PP-OCRv6_rec_tiny.onnx");
	auto [detData, detSize] = Ling::Util::getRes(L"PP-OCRv6_det_tiny.onnx");

	tinyocr::Options opts;
	opts.det_model_data = detData;
	opts.det_model_size = detSize;
	opts.rec_model_data = recData;
	opts.rec_model_size = recSize;
	opts.return_word_box = true;
	opts.return_single_char_box = true;

	tinyocr::TinyOcr ocr(opts);
	auto result = ocr.run(bgr.data(), w, h, 3);
	// OcrDoc 负责把 word box 对齐到行文本的字符下标，并预建每字符的路径几何
	doc.loadResult(result);
	auto cur = dynamic_cast<WindowMain*>(win);
	cur->textBox->loadText(&doc);
}
