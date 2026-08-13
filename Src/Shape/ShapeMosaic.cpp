#include "pch.h"
#include "Win/WinPin.h"
#include "Tool/ToolMain.h"
#include "Tool/ToolSub.h"
#include "History.h"
#include "ShapeMosaic.h"

using Microsoft::WRL::ComPtr;

ShapeMosaic::ShapeMosaic(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0)
}
{
	auto toolSub = win->toolSub.get();
	auto d2d = Ling::D2D::get();
	// 马赛克没有颜色可选，这个半透明品红只在拖拽过程中当占位提示 ——
	// 松开鼠标后 buildMosaicBitmap 会算出真正的马赛克画刷把它替换掉
	d2d->deviceContext->CreateSolidColorBrush(D2D1::ColorF(0xF00FF0, 0.38f), brush.GetAddressOf());
	strokeWidth = toolSub->getSliderVal();
	isRect = toolSub->isMosaicRect;
	// 圆头圆角描边。本项目没有 App::getRoundStrokeStyle 那样的全局缓存，
	// 就每个 shape 自己持一个 —— StrokeStyle 是不可变的轻对象，代价可以忽略
	d2d->d2dFactory->CreateStrokeStyle(
		D2D1::StrokeStyleProperties(
			D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND, D2D1_CAP_STYLE_ROUND,
			D2D1_LINE_JOIN_ROUND, 8.f, D2D1_DASH_STYLE_SOLID, 0.f
		),
		nullptr, 0, roundStyle.GetAddressOf());
}

ShapeMosaic::~ShapeMosaic()
{

}

void ShapeMosaic::paint(ID2D1DeviceContext* ctx)
{
	if (isRect) {
		if (mosaicBrush) {
			ctx->FillRectangle(rect, mosaicBrush.Get());
		}
		else {
			ctx->FillRectangle(rect, brush.Get());
		}
		return;
	}
	// makePath 要等第一次 mouseDown 才建 path，这之前可能先来一次 paint
	if (!path) return;
	if (mosaicBrush) {
		ctx->DrawGeometry(path.Get(), mosaicBrush.Get(), strokeWidth, roundStyle.Get());
	}
	else {
		ctx->DrawGeometry(path.Get(), brush.Get(), strokeWidth, roundStyle.Get());
	}
}

void ShapeMosaic::paintDragger(ID2D1DeviceContext* ctx)
{
	// 矩形模式 8 个夹点，涂抹模式只有首尾 2 个
	size_t count = isRect ? draggers.size() : 2;
	for (size_t i = 0; i < count; ++i)
	{
		ctx->DrawRectangle(draggers[i], brushDragger.Get(), win->dpi);
	}
}

void ShapeMosaic::mouseDrag(const float x, const float y)
{
	bool shiftDown = (GetKeyState(VK_SHIFT) & 0x8000) != 0;
	if (isRect) {
		// 几何一变，之前算好的马赛克就不对了，先扔掉退回占位色
		resetMosaic();
		if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4 || hoverDraggerIndex == 2 || hoverDraggerIndex == 6) {
			auto [left, right] = std::minmax(pressX, x);
			auto [top, bottom] = std::minmax(pressY, y);
			rect.left = left;
			rect.right = right;
			rect.top = top;
			rect.bottom = bottom;
		}
		else if (hoverDraggerIndex == 1 || hoverDraggerIndex == 5) {
			auto [top, bottom] = std::minmax(pressY, y);
			rect.top = top;
			rect.bottom = bottom;
		}
		else if (hoverDraggerIndex == 3 || hoverDraggerIndex == 7) {
			auto [left, right] = std::minmax(pressX, x);
			rect.left = left;
			rect.right = right;
		}
		else if (hoverDraggerIndex == 8) {
			auto w = rect.right - rect.left;
			auto h = rect.bottom - rect.top;
			rect.left = x - pressX;
			rect.top = y - pressY;
			rect.right = rect.left + w;
			rect.bottom = rect.top + h;
		}
		if (hoverDraggerIndex != 8 && shiftDown) {
			auto w = rect.right - rect.left;
			auto h = rect.bottom - rect.top;
			if (w > h) {
				rect.bottom = rect.top + w;
			}
			else {
				rect.right = rect.left + h;
			}
		}
		return;
	}
	// Shift 按下时不再追加顶点，而是拖动端点 —— 效果是一条直线；松开 Shift 就是自由涂抹
	if (hoverDraggerIndex == 0) {
		if (shiftDown) {
			auto& p = linePoints[0];
			p.x = x;
			p.y = y;
		}
		else {
			linePoints.insert(linePoints.begin(), { x,y });
		}
		makePath();
	}
	else if (hoverDraggerIndex == 1) {
		if (shiftDown) {
			auto& p = linePoints[linePoints.size() - 1];
			p.x = x;
			p.y = y;
		}
		else {
			linePoints.push_back({ x,y });
		}
		makePath();
	}
	else if (hoverDraggerIndex == 8) {
		auto spanX{ x - pressX };
		auto spanY{ y - pressY };
		for (auto& p : linePoints)
		{
			p.x += spanX;
			p.y += spanY;
		}
		makePath();
		pressX = x;
		pressY = y;
	}
}

void ShapeMosaic::mouseDown(const float x, const float y)
{
	if (isRect) {
		if (hoverDraggerIndex == -1) { //首次创建
			pressX = x;
			pressY = y;
			rect = D2D1::RectF(x, y, x, y);
			hoverDraggerIndex = 4;
		}
		else if (hoverDraggerIndex == 0) {
			pressX = rect.right;
			pressY = rect.bottom;
		}
		else if (hoverDraggerIndex == 1) {
			pressY = rect.bottom;
		}
		else if (hoverDraggerIndex == 2) {
			pressX = rect.left;
			pressY = rect.bottom;
		}
		else if (hoverDraggerIndex == 3) {
			pressX = rect.left;
		}
		else if (hoverDraggerIndex == 4) {
			pressX = rect.left;
			pressY = rect.top;
		}
		else if (hoverDraggerIndex == 5) {
			pressY = rect.top;
		}
		else if (hoverDraggerIndex == 6) {
			pressX = rect.right;
			pressY = rect.top;
		}
		else if (hoverDraggerIndex == 7) {
			pressX = rect.right;
		}
		else if (hoverDraggerIndex == 8) {
			pressX = x - rect.left;
			pressY = y - rect.top;
		}
		return;
	}
	if (hoverDraggerIndex == -1) { //首次创建
		linePoints.push_back({ x,y });
		if (GetKeyState(VK_SHIFT) & 0x8000) {
			// 直线模式需要两个点，末点跟着鼠标走
			linePoints.push_back({ x,y });
		}
		makePath();
		hoverDraggerIndex = 1;
		win->refresh();
	}
	else if (hoverDraggerIndex == 8) {
		pressX = x;
		pressY = y;
	}
}

void ShapeMosaic::mouseUp(const float x, const float y)
{
	updateDraggers();
	// 几何定下来了才算马赛克 —— 这一步要把 GPU 像素读回内存，拖拽过程中每帧做太贵
	buildMosaicBitmap();
}

void ShapeMosaic::mouseMove(const float x, const float y)
{
	hoverDraggerIndex = -1;
	if (isRect) {
		for (size_t i = 0; i < draggers.size(); ++i) {
			if (isInRect(draggers[i], x, y)) {
				hoverDraggerIndex = (int)i;
				return;
			}
		}
		auto half{ strokeWidth / 2.f + win->dpi };//多个一个dpi，让范围更大点
		if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half)
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
		}
		return;
	}
	if (isInRect(draggers[0], x, y))
	{
		hoverDraggerIndex = 0;
	}
	else if (isInRect(draggers[1], x, y))
	{
		hoverDraggerIndex = 1;
	}
	if (hoverDraggerIndex == -1)
	{
		hitTest({ x,y });
	}
}

void ShapeMosaic::setCursor()
{
	if (isRect) {
		if (hoverDraggerIndex == 0 || hoverDraggerIndex == 4) {
			SetCursor(LoadCursor(nullptr, IDC_SIZENWSE));
		}
		else if (hoverDraggerIndex == 1 || hoverDraggerIndex == 5) {
			SetCursor(LoadCursor(nullptr, IDC_SIZENS));
		}
		else if (hoverDraggerIndex == 2 || hoverDraggerIndex == 6) {
			SetCursor(LoadCursor(nullptr, IDC_SIZENESW));
		}
		else if (hoverDraggerIndex == 3 || hoverDraggerIndex == 7) {
			SetCursor(LoadCursor(nullptr, IDC_SIZEWE));
		}
		else if (hoverDraggerIndex == 8) {
			SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
		}
		return;
	}
	if (hoverDraggerIndex == 8) {
		SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
	}
}

void ShapeMosaic::makePath()
{
	if (linePoints.empty()) return;
	resetMosaic();
	auto d2d = Ling::D2D::get();
	// ReleaseAndGetAddressOf 而不是 GetAddressOf：后者不放旧对象，自由涂抹时每个鼠标事件漏一个几何体
	d2d->d2dFactory->CreatePathGeometry(path.ReleaseAndGetAddressOf());
	if (!path) return;
	ComPtr<ID2D1GeometrySink> sink;
	path->Open(sink.GetAddressOf());
	sink->BeginFigure(linePoints[0], D2D1_FIGURE_BEGIN_HOLLOW);
	if (linePoints.size() > 1) {
		sink->AddLines(&linePoints[1], (UINT32)(linePoints.size() - 1));
	}
	sink->EndFigure(D2D1_FIGURE_END_OPEN);
	sink->Close();
}

void ShapeMosaic::resetMosaic()
{
	mosaicBitmap.Reset();
	mosaicBrush.Reset();
	mosaicOrigin = { 0.f, 0.f };
}

void ShapeMosaic::updateDraggers()
{
	auto half{ draggerSize / 2 };
	if (isRect) {
		auto w{ rect.right - rect.left }, h{ rect.bottom - rect.top };
		draggers[0] = D2D1::RectF(rect.left - half, rect.top - half, rect.left + half, rect.top + half);
		draggers[1] = D2D1::RectF(rect.left + w / 2 - half, rect.top - half, rect.left + w / 2 + half, rect.top + half);
		draggers[2] = D2D1::RectF(rect.right - half, rect.top - half, rect.right + half, rect.top + half);
		draggers[3] = D2D1::RectF(rect.right - half, rect.top + h / 2 - half, rect.right + half, rect.top + h / 2 + half);
		draggers[4] = D2D1::RectF(rect.right - half, rect.bottom - half, rect.right + half, rect.bottom + half);
		draggers[5] = D2D1::RectF(rect.left + w / 2 - half, rect.bottom - half, rect.left + w / 2 + half, rect.bottom + half);
		draggers[6] = D2D1::RectF(rect.left - half, rect.bottom - half, rect.left + half, rect.bottom + half);
		draggers[7] = D2D1::RectF(rect.left - half, rect.top + h / 2 - half, rect.left + half, rect.top + h / 2 + half);
		return;
	}
	if (linePoints.empty()) return;
	auto& start = linePoints[0];
	draggers[0].left = start.x - half;
	draggers[0].top = start.y - half;
	draggers[0].right = start.x + half;
	draggers[0].bottom = start.y + half;
	auto& end = linePoints[linePoints.size() - 1];
	draggers[1].left = end.x - half;
	draggers[1].top = end.y - half;
	draggers[1].right = end.x + half;
	draggers[1].bottom = end.y + half;
}

void ShapeMosaic::buildMosaicBitmap()
{
	if (!isRect && !path) return;
	if (isRect && (rect.right <= rect.left || rect.bottom <= rect.top)) return;
	// 块越大越糊。滑块调的是笔画粗细，顺带让粗笔画对应大色块，下限 6px 保证肉眼可见
	int blockSize = std::max(6, (int)std::round(strokeWidth / 3.0f));
	mosaicBitmap = createMosaicBitmap(blockSize);
	mosaicBrush.Reset();
	if (!mosaicBitmap) return;
	// NEAREST_NEIGHBOR 是关键：线性插值会把色块边界糊成渐变，就不像马赛克了
	auto bitmapBrushProps = D2D1::BitmapBrushProperties(
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	auto brushProps = D2D1::BrushProperties();
	auto d2d = Ling::D2D::get();
	d2d->deviceContext->CreateBitmapBrush(mosaicBitmap.Get(), &bitmapBrushProps, &brushProps, mosaicBrush.GetAddressOf());
	if (mosaicBrush) {
		// 位图只覆盖包围盒那一小块，画刷默认从原点铺，要平移到包围盒左上角才对得上
		mosaicBrush->SetTransform(D2D1::Matrix3x2F::Translation(mosaicOrigin.x, mosaicOrigin.y));
	}
}

// 把"这个 shape 之前"的画面在包围盒范围内重现一遍，读回像素打成马赛克，再包成画刷。
// 用 d2d->deviceContext 做离屏绘制是安全的：它是全项目共享的资源工厂，
// 没有任何地方给它 SetTarget / BeginDraw（窗口绘制走的是各自 surface 或 swap chain 的 context）。
// 这里 SetTarget → BeginDraw → EndDraw → SetTarget(nullptr) 在函数内闭环，不跨帧持有。
ComPtr<ID2D1Bitmap> ShapeMosaic::createMosaicBitmap(int blockSize)
{
	ComPtr<ID2D1Bitmap> result;
	if ((!isRect && !path) || win->w <= 0 || win->h <= 0) return result;
	if (!win->screenImg) return result;

	auto d2d = Ling::D2D::get();
	auto ctx = d2d->deviceContext.Get();

	D2D1_RECT_F bounds{};
	HRESULT hr{ S_OK };
	if (isRect) {
		bounds = rect;
	}
	else {
		// 描边后的实际覆盖范围，比 path 本身宽 strokeWidth
		hr = path->GetWidenedBounds(strokeWidth, roundStyle.Get(), nullptr, &bounds);
		if (FAILED(hr)) return result;
	}

	// 往外扩一个 blockSize，避免边缘那一列色块取样不足；再夹到窗口范围内
	const int winW = (int)win->w, winH = (int)win->h;
	int left = std::max(0, std::min((int)std::floor(bounds.left) - blockSize, winW));
	int top = std::max(0, std::min((int)std::floor(bounds.top) - blockSize, winH));
	int right = std::max(0, std::min((int)std::ceil(bounds.right) + blockSize + 1, winW));
	int bottom = std::max(0, std::min((int)std::ceil(bounds.bottom) + blockSize + 1, winH));
	if (left >= right || top >= bottom) return result;

	mosaicOrigin = { (float)left, (float)top };
	auto localSize = D2D1::SizeU((UINT32)(right - left), (UINT32)(bottom - top));

	D2D1_BITMAP_PROPERTIES1 targetProps{
		.pixelFormat{ D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED) },
		.dpiX{ 96.0f }, .dpiY{ 96.0f },
		.bitmapOptions{ D2D1_BITMAP_OPTIONS_TARGET }
	};
	ComPtr<ID2D1Bitmap1> targetBitmap;
	hr = ctx->CreateBitmap(localSize, nullptr, 0, &targetProps, targetBitmap.GetAddressOf());
	if (FAILED(hr)) return result;

	// 底图 + 排在自己前面且没被撤销的 shape。平移变换让窗口坐标直接落到这块小位图里，
	// 各 shape 的 paint 不用知道自己被画到了别处
	ctx->SetTarget(targetBitmap.Get());
	ctx->SetTransform(D2D1::Matrix3x2F::Translation(-mosaicOrigin.x, -mosaicOrigin.y));
	ctx->BeginDraw();
	ctx->Clear(D2D1::ColorF(0, 0.0f));
	ctx->DrawBitmap(win->screenImg.Get(), D2D1::RectF(0, 0, win->w, win->h));
	for (auto& shape : win->history->shapes)
	{
		auto cur = shape.get();
		if (cur == this) break;
		if (!cur->isUndo) {
			cur->paint(ctx);
		}
	}
	hr = ctx->EndDraw();
	ctx->SetTransform(D2D1::Matrix3x2F::Identity());
	// 解绑，下面 CopyFromBitmap 才能把它当源读
	ctx->SetTarget(nullptr);
	if (FAILED(hr)) return result;

	// GPU 上的 target 位图不能直接 Map，得先拷到一块带 CPU_READ 的位图上
	D2D1_BITMAP_PROPERTIES1 cpuProps{
		.pixelFormat{ targetBitmap->GetPixelFormat() },
		.dpiX{ 96.0f }, .dpiY{ 96.0f },
		.bitmapOptions{ D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW }
	};
	ComPtr<ID2D1Bitmap1> cpuBitmap;
	hr = ctx->CreateBitmap(localSize, nullptr, 0, &cpuProps, cpuBitmap.GetAddressOf());
	if (FAILED(hr)) return result;

	hr = cpuBitmap->CopyFromBitmap(nullptr, targetBitmap.Get(), nullptr);
	if (FAILED(hr)) return result;

	D2D1_MAPPED_RECT mapped{};
	hr = cpuBitmap->Map(D2D1_MAP_OPTIONS_READ, &mapped);
	if (FAILED(hr)) return result;

	std::vector<BYTE> pixels((size_t)mapped.pitch * localSize.height);
	CopyMemory(pixels.data(), mapped.bits, pixels.size());
	cpuBitmap->Unmap();

	mosaicPixels(pixels.data(), mapped.pitch, localSize.width, localSize.height, blockSize);

	auto bitmapProps = D2D1::BitmapProperties(D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
	hr = ctx->CreateBitmap(localSize, pixels.data(), mapped.pitch, &bitmapProps, result.GetAddressOf());
	return result;
}

// 按 blockSize 分块，每块取平均色再整块填回去 —— 就是马赛克
void ShapeMosaic::mosaicPixels(BYTE* bits, UINT32 pitch, UINT32 width, UINT32 height, int blockSize)
{
	if (!bits || blockSize <= 0 || width == 0 || height == 0) return;

	for (UINT32 y = 0; y < height; y += blockSize) {
		UINT32 yEnd = std::min(y + (UINT32)blockSize, height);
		for (UINT32 x = 0; x < width; x += blockSize) {
			UINT32 xEnd = std::min(x + (UINT32)blockSize, width);
			unsigned long long bSum{ 0 }, gSum{ 0 }, rSum{ 0 }, aSum{ 0 }, count{ 0 };
			for (UINT32 yy = y; yy < yEnd; ++yy) {
				auto row = bits + yy * pitch;
				for (UINT32 xx = x; xx < xEnd; ++xx) {
					auto pixel = row + xx * 4;
					bSum += pixel[0];
					gSum += pixel[1];
					rSum += pixel[2];
					aSum += pixel[3];
					++count;
				}
			}
			if (count == 0) continue;
			BYTE b = (BYTE)(bSum / count);
			BYTE g = (BYTE)(gSum / count);
			BYTE r = (BYTE)(rSum / count);
			BYTE a = (BYTE)(aSum / count);
			for (UINT32 yy = y; yy < yEnd; ++yy) {
				auto row = bits + yy * pitch;
				for (UINT32 xx = x; xx < xEnd; ++xx) {
					auto pixel = row + xx * 4;
					pixel[0] = b;
					pixel[1] = g;
					pixel[2] = r;
					pixel[3] = a;
				}
			}
		}
	}
}

float  ShapeMosaic::pointToSegmentDistance(const D2D1_POINT_2F& p, const D2D1_POINT_2F& a, const D2D1_POINT_2F& b)
{
	float abx = b.x - a.x, aby = b.y - a.y;
	float apx = p.x - a.x, apy = p.y - a.y;
	float ab2 = abx * abx + aby * aby;
	float t = (ab2 == 0.0f) ? 0.0f : (apx * abx + apy * aby) / ab2;
	t = std::max(0.0f, std::min(1.0f, t));  // 限制在线段范围内
	float cx = a.x + t * abx;
	float cy = a.y + t * aby;
	float dx = p.x - cx, dy = p.y - cy;

	return std::sqrtf(dx * dx + dy * dy);
}

void ShapeMosaic::hitTest(const D2D1_POINT_2F& mousePos)
{
	// 只有笔画边缘那一圈算命中：粗笔画（马赛克、橡皮擦尤其明显）整片都能拖的话，
	// 就没法在已有笔画上面再画一笔了 —— 鼠标一按下会变成拖动旧元素。
	// 矩形模式本来就是只有边框附近才响应，这里跟它对齐
	float outer = strokeWidth * 0.5f + win->dpi; //外沿保持原来的判定范围
	float inner = outer - draggerSize / 2; //往里让出这么宽算内部。细笔画算出来是负数，等于整条线都能拖
	// 得先在所有线段里取最小距离再判断：自交的笔画里，某一段的边缘可能正好压在另一段的
	// 内部，那种位置视觉上是在笔画里面，逐段判断会误判成边缘
	float minDist{ FLT_MAX };
	for (size_t i = 0; i + 1 < linePoints.size(); ++i) {
		minDist = std::min(minDist, pointToSegmentDistance(mousePos, linePoints[i], linePoints[i + 1]));
	}
	if (minDist <= outer && minDist >= inner) {
		hoverDraggerIndex = 8;
	}
}
