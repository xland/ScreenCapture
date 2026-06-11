#include "pch.h"
#include "App.h"
#include "Win/WinPin.h"
#include "Tool/WinToolMain.h"
#include "Tool/WinToolSub.h"
#include "Tool/WinToolSubSlider.h"
#include "History.h"
#include "Util.h"
#include "ShapeMosaic.h"

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
	win->render->CreateSolidColorBrush(D2D1::ColorF(0xF00FF0, 0.38f), brush.GetAddressOf());
	strokeWidth = toolSub->slider->val;
	isRect = toolSub->selectIndex == 0;
}

ShapeMosaic::~ShapeMosaic()
{

}

void ShapeMosaic::paint()
{
	if (isRect) {
		if (mosaicBrush.Get()) {
			win->render->FillRectangle(rect, mosaicBrush.Get());
		}
		else {
			win->render->FillRectangle(rect, brush.Get());
		}
		return;
	}
	if (!path.Get()) return;
	if (mosaicBrush.Get()) {
		win->render->DrawGeometry(path.Get(), mosaicBrush.Get(), strokeWidth, getRoundStrokeStyle());
	}
	else {
		win->render->DrawGeometry(path.Get(), brush.Get(), strokeWidth, getRoundStrokeStyle());
	}
}

void ShapeMosaic::paintDragger()
{
	size_t count = isRect ? draggers.size() : 2;
	for (size_t i = 0; i < count; ++i)
	{
		win->render->DrawRectangle(draggers[i], brushDragger.Get(), win->dpi);
	}
}

void ShapeMosaic::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
	if (isRect) {
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
		if (hoverDraggerIndex != 8 && (modifiers & MK_SHIFT) != 0) {
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
	if (hoverDraggerIndex == 0) {
		if ((modifiers & MK_SHIFT) != 0) {
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
		if ((modifiers & MK_SHIFT) != 0) {
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
		for (auto& p:linePoints)
		{
			p.x += spanX;
			p.y += spanY;
		}
		makePath();
		pressX = x;
		pressY = y;
	}
}

void ShapeMosaic::mouseDown(const float& x, const float& y)
{
	if (isRect) {
		if (hoverDraggerIndex == -1) {
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
	if (hoverDraggerIndex == -1) {
		linePoints.push_back({ x,y }); 
		if (GetKeyState(VK_SHIFT) & 0x8000) {
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

void ShapeMosaic::mouseUp(const float& x, const float& y)
{
	updateDraggers();
	buildMosaicBitmap();
}

void ShapeMosaic::mouseMove(const float& x, const float& y)
{
    hoverDraggerIndex = -1;
	if (isRect) {
		for (size_t i = 0; i < draggers.size(); ++i) {
			if (Util::isInRect(draggers[i], x, y)) {
				hoverDraggerIndex = (int)i;
				return;
			}
		}
		auto half{ strokeWidth / 2.f + win->dpi };
		if (x >= rect.left - half && x <= rect.right + half && y >= rect.top - half && y <= rect.bottom + half)
		{
			if (x <= rect.left + half || x >= rect.right - half || y <= rect.top + half || y >= rect.bottom - half) {
				hoverDraggerIndex = 8;
			}
		}
		return;
	}
    if (Util::isInRect(draggers[0], x, y))
    {
        hoverDraggerIndex = 0;
    }
    else if (Util::isInRect(draggers[1], x, y))
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
			win->setCursor(IDC_SIZENWSE);
		}
		else if (hoverDraggerIndex == 1 || hoverDraggerIndex == 5) {
			win->setCursor(IDC_SIZENS);
		}
		else if (hoverDraggerIndex == 2 || hoverDraggerIndex == 6) {
			win->setCursor(IDC_SIZENESW);
		}
		else if (hoverDraggerIndex == 3 || hoverDraggerIndex == 7) {
			win->setCursor(IDC_SIZEWE);
		}
		else if (hoverDraggerIndex == 8) {
			win->setCursor(IDC_SIZEALL);
		}
		return;
	}
	if (hoverDraggerIndex == 8) {
		win->setCursor(IDC_SIZEALL);
	}
}

void ShapeMosaic::makePath()
{
	resetMosaic();
	path.Reset();
	App::getD2D()->CreatePathGeometry(path.GetAddressOf());
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
	if (!isRect && !path.Get()) return;
	if (isRect && (rect.right <= rect.left || rect.bottom <= rect.top)) return;
	int blockSize = std::max(6, (int)std::round(strokeWidth / 3.0f));
	mosaicBitmap = createMosaicBitmap(blockSize);
	mosaicBrush.Reset();
	if (!mosaicBitmap.Get()) return;
	auto bitmapBrushProps = D2D1::BitmapBrushProperties(
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_EXTEND_MODE_CLAMP,
		D2D1_BITMAP_INTERPOLATION_MODE_NEAREST_NEIGHBOR);
	auto brushProps = D2D1::BrushProperties();
	win->render->CreateBitmapBrush(mosaicBitmap.Get(), &bitmapBrushProps, &brushProps, mosaicBrush.GetAddressOf());
	if (mosaicBrush.Get()) {
		mosaicBrush->SetTransform(D2D1::Matrix3x2F::Translation(mosaicOrigin.x, mosaicOrigin.y));
	}
}

ComPtr<ID2D1Bitmap> ShapeMosaic::createMosaicBitmap(int blockSize)
{
	ComPtr<ID2D1Bitmap> result;
	if ((!isRect && !path.Get()) || win->w <= 0 || win->h <= 0) return result;

	D2D1_RECT_F bounds{};
	HRESULT hr{ S_OK };
	if (isRect) {
		bounds = rect;
	}
	else {
		hr = path->GetWidenedBounds(strokeWidth, getRoundStrokeStyle(), nullptr, &bounds);
		if (FAILED(hr)) return result;
	}

	int left = std::max(0, std::min((int)std::floor(bounds.left) - blockSize, win->w));
	int top = std::max(0, std::min((int)std::floor(bounds.top) - blockSize, win->h));
	int right = std::max(0, std::min((int)std::ceil(bounds.right) + blockSize + 1, win->w));
	int bottom = std::max(0, std::min((int)std::ceil(bounds.bottom) + blockSize + 1, win->h));
	if (left >= right || top >= bottom) return result;

	mosaicOrigin = { (float)left, (float)top };
	auto localSize = D2D1::SizeU((UINT32)(right - left), (UINT32)(bottom - top));

	D2D1_BITMAP_PROPERTIES1 targetProps{};
	targetProps.pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED);
	targetProps.bitmapOptions = D2D1_BITMAP_OPTIONS_TARGET;
	targetProps.dpiX = 96.0f;
	targetProps.dpiY = 96.0f;
	ComPtr<ID2D1Bitmap1> targetBitmap;
	hr = win->render->CreateBitmap(localSize, nullptr, 0, &targetProps, targetBitmap.GetAddressOf());
	if (FAILED(hr)) return result;

	ComPtr<ID2D1Image> oldTarget;
	D2D1_MATRIX_3X2_F oldTransform{};
	win->render->GetTarget(oldTarget.GetAddressOf());
	win->render->GetTransform(&oldTransform);
	win->render->SetTarget(targetBitmap.Get());
	win->render->SetTransform(D2D1::Matrix3x2F::Translation(-mosaicOrigin.x, -mosaicOrigin.y));
	win->render->BeginDraw();
	win->render->Clear(D2D1::ColorF(0, 0.0f));
	win->render->DrawBitmap(win->screenImg.Get(), D2D1::RectF(0, 0, (float)win->w, (float)win->h));
	for (auto& shape : win->history->shapes)
	{
		auto cur = shape.get();
		if (cur == this) break;
		if (!cur->isUndo) {
			cur->paint();
		}
	}
	hr = win->render->EndDraw();
	win->render->SetTransform(oldTransform);
	win->render->SetTarget(oldTarget.Get());
	if (FAILED(hr)) return result;

	D2D1_BITMAP_PROPERTIES1 cpuProps{};
	cpuProps.pixelFormat = targetBitmap->GetPixelFormat();
	cpuProps.bitmapOptions = D2D1_BITMAP_OPTIONS_CPU_READ | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
	cpuProps.dpiX = 96.0f;
	cpuProps.dpiY = 96.0f;
	ComPtr<ID2D1Bitmap1> cpuBitmap;
	hr = win->render->CreateBitmap(localSize, nullptr, 0, &cpuProps, cpuBitmap.GetAddressOf());
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
	hr = win->render->CreateBitmap(localSize, pixels.data(), mapped.pitch, &bitmapProps, result.GetAddressOf());
	return result;
}

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
	float half = strokeWidth * 0.5f + win->dpi;
	for (size_t i = 0; i + 1 < linePoints.size(); ++i) {
		if (pointToSegmentDistance(mousePos, linePoints[i], linePoints[i + 1]) <= half) {
			hoverDraggerIndex = 8;
		}
	}
}
