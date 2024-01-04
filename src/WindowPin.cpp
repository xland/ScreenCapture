#include <windowsx.h>
#include "WindowPin.h"
#include "WindowMain.h"
#include "CutMask.h"
#include "include/core/SkPath.h"
#include "include/core/SkRect.h"
#include "include/core/SkSurface.h"
#include "include/core/SkCanvas.h"
#include "include/utils/SkShadowUtils.h"
#include "include/core/SkPoint3.h"
#include "include/core/SkRRect.h"
#include "include/core/SkStream.h"
#include "include/encode/SkPngEncoder.h"


WindowPin* windowPin;

WindowPin::WindowPin()
{
	float shadowSize = 20.0f;
	//auto rect = CutMask::GetCutRect();
    auto rect = SkRect::MakeLTRB(200, 200, 600, 600);
	x = rect.fLeft - shadowSize;
	y = rect.fTop - shadowSize;
	w = rect.width() + shadowSize * 2;
	h = rect.height() + shadowSize * 2;
	SkImageInfo imgInfo = SkImageInfo::MakeN32Premul(w, h);
	long long rowBytes = w * 4;
	long long dataSize = rowBytes * h;
	auto pixArr = new unsigned char[dataSize];
    pixSrc = new SkPixmap(imgInfo, pixArr, rowBytes);

    //auto canvas = SkCanvas::MakeRasterDirect(imgInfo, pixArr, rowBytes);
	//auto windowMain = WindowMain::get();
	//auto img = windowMain->surfaceBase->makeImageSnapshot(SkIRect::MakeLTRB(rect.fLeft, rect.fTop, rect.fRight, rect.fBottom));
	//canvas->drawImage(img, shadowSize, shadowSize);
	//SkPaint paint;
	//SkPath path;
	//path.addRRect(SkRRect::MakeRectXY(SkRect::MakeXYWH(shadowSize, shadowSize, rect.width(),rect.height()), 6, 6));
	//// 定义阴影与 z 平面的关系
	//SkPoint3 zPlaneParams = SkPoint3::Make(0, 0, 20);
	//// 定义光源的位置和半径
	//SkPoint3 lightPos = SkPoint3::Make(0, 0, 0);
	//SkScalar lightRadius = 20;
	//// 绘制阴影
	//SkShadowUtils::DrawShadow(canvas.get(), path, zPlaneParams, lightPos, lightRadius, SkColorSetARGB(130, 0, 0, 0), SK_ColorTRANSPARENT, 0);

    initWindow();
    initCanvas();
}

WindowPin::~WindowPin()
{
    windowPin = nullptr;
}

void WindowPin::init()
{
	if (!windowPin) {
		windowPin = new WindowPin();
		windowPin->Show();
		//WindowMain::get()->Close(0);
	}
}

WindowPin* WindowPin::get()
{
	return windowPin;
}

LRESULT WindowPin::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    switch (msg)
    {
    case WM_LBUTTONDOWN:
    {
        IsMouseDown = true;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseDown(x, y);
    }
    case WM_LBUTTONUP:
    {
        IsMouseDown = false;
        IsMouseDragging = false;
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        return onMouseUp(x, y);
    }
    case WM_MOUSEMOVE:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        if (IsMouseDown) {
            IsMouseDragging = true;
            return onMouseDrag(x, y);
        }
        else {
            return onMouseMove(x, y);
        }
    }
    case WM_RBUTTONDOWN:
    {
        auto x = GET_X_LPARAM(lparam);
        auto y = GET_Y_LPARAM(lparam);
        onMouseDownRight(x, y);
        return true;
    }
    case WM_KEYDOWN:
    {
        switch (wparam)
        {
        case VK_ESCAPE:
        {
            this->Close(3);
            return false;
        }
        default:
        {
            onKeyDown(wparam);
        }
        }
        return false;
    }
    case WM_CHAR: {
        return onChar(wparam);
    }
    case WM_MOUSEWHEEL: {
        int delta = GET_WHEEL_DELTA_WPARAM(wparam);
        onMouseWheel(delta);
        return false;
    }
    default:
        break;
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

void WindowPin::paintTool(SkCanvas* canvas)
{
    SkPaint paint;
    paint.setColor(SK_ColorBLUE);
    canvas->drawRect(SkRect::MakeLTRB(0, 0, w, h), paint);

    SkFILEWStream stream("output123.bmp");
    SkPngEncoder::Options option;
    SkPngEncoder::Encode(&stream, *pixBase, option);
    stream.flush();
}

bool WindowPin::onMouseDown(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseDownRight(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseUp(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseMove(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onMouseDrag(const int& x, const int& y)
{
	return false;
}

bool WindowPin::onChar(const unsigned int& val)
{
	return false;
}

bool WindowPin::onKeyDown(const unsigned int& val)
{
	return false;
}

bool WindowPin::onMouseWheel(const int& delta)
{
	return false;
}
