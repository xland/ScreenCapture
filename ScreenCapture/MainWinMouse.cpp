#include "MainWin.h"

void MainWin::leftBtnDown(const POINT& pos)
{
    isLeftBtnDown = true;
    mouseDownPos = pos;
}
void MainWin::rightBtnDown(const POINT& pos)
{
    CloseWindow(hwnd);
	PostQuitMessage(0);
}
void MainWin::mouseMove(const POINT& pos)
{
    if (!isLeftBtnDown) return;
    if (state == State::Start) {
        if (pos.x < mouseDownPos.x)
        {
            cutRect.left = pos.x;
            cutRect.right = mouseDownPos.x;
        }
        else
        {
            cutRect.right = pos.x;
            cutRect.left = mouseDownPos.x;
        }
        if (pos.y < mouseDownPos.y)
        {
            cutRect.top = pos.y;
            cutRect.bottom = mouseDownPos.y;
        }
        else
        {
            cutRect.top = mouseDownPos.y;
            cutRect.bottom = pos.y;
        }
    }
    else if (state == State::Eraser) {
        if (!eraserImg.Get())
        {
            unsigned int dataSize = w * h * 4;
            char* pixels = new char[dataSize]; //透明部分被遮住，不透明部分露出
            //std::fill(pixels[0], pixels[dataSize - 1], 255);
            memset(pixels, 0, dataSize);
            auto props = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET, D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED));
            context->CreateBitmap(D2D1::SizeU(w, h), pixels, 4 * w, props, &eraserImg);
            delete[] pixels;
        }
        static ComPtr<ID2D1SolidColorBrush> brush;
        if (!brush.Get()) {
            context->CreateSolidColorBrush(D2D1::ColorF(0.0f, 0.0f, 0.0f, 1.0f), &brush);
        }
        static ComPtr<ID2D1StrokeStyle1> strokeStyle;
        if (strokeStyle.Get()) {
            d2dfactory1->CreateStrokeStyle(
                D2D1::StrokeStyleProperties1(
                    D2D1_CAP_STYLE_ROUND,
                    D2D1_CAP_STYLE_ROUND,
                    D2D1_CAP_STYLE_ROUND,
                    D2D1_LINE_JOIN_ROUND,
                    10.0f,
                    D2D1_DASH_STYLE_SOLID,
                    0.0f, D2D1_STROKE_TRANSFORM_TYPE_FIXED),
                nullptr, 0, &strokeStyle);
        }

        context->SetTarget(eraserImg.Get());
        context->BeginDraw();
        context->DrawLine(D2D1::Point2F((float)mouseDownPos.x, (float)mouseDownPos.y),
            D2D1::Point2F((float)pos.x, (float)pos.y), brush.Get(), 18.0f, strokeStyle.Get());
        context->EndDraw();
        context->Flush();
        mouseDownPos = pos;
    }
    paint();
    InvalidateRect(hwnd, nullptr, false);
}
void MainWin::leftBtnUp(const POINT& pos)
{
    isLeftBtnDown = false;
    if (state == State::Start) {
        state = State::Eraser;
    }
    else if (state == State::Eraser) {
        
    }
}