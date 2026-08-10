#pragma once
#include <include/Ling.h>
#include "ShapeBase.h"
class ShapeMosaic : public ShapeBase
{
public:
	ShapeMosaic(WinPin* win);
	~ShapeMosaic();
	void paint(ID2D1DeviceContext* ctx) override;
	void paintDragger(ID2D1DeviceContext* ctx) override;
	void mouseDrag(const float x, const float y) override;
	void mouseDown(const float x, const float y) override;
	void mouseUp(const float x, const float y) override;
	void mouseMove(const float x, const float y) override;
	void setCursor() override;
public:
private:
	void makePath();
	void resetMosaic();
	void updateDraggers();
	void buildMosaicBitmap();
	Microsoft::WRL::ComPtr<ID2D1Bitmap> createMosaicBitmap(int blockSize);
	void mosaicPixels(BYTE* bits, UINT32 pitch, UINT32 width, UINT32 height, int blockSize);
	float pointToSegmentDistance(const D2D1_POINT_2F& p, const D2D1_POINT_2F& a, const D2D1_POINT_2F& b);
	void hitTest(const D2D1_POINT_2F& mousePos);
private:
	D2D1_RECT_F rect{ 0.f,0.f,0.f,0.f };
	std::vector<D2D1_RECT_F> draggers;
	std::vector<D2D1_POINT_2F> linePoints;
	Microsoft::WRL::ComPtr<ID2D1PathGeometry> path;
	// 圆头圆角描边，涂抹模式下让笔画首尾和拐点是圆的
	Microsoft::WRL::ComPtr<ID2D1StrokeStyle> roundStyle;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brush;
	Microsoft::WRL::ComPtr<ID2D1Bitmap> mosaicBitmap;
	Microsoft::WRL::ComPtr<ID2D1BitmapBrush> mosaicBrush;
	// 马赛克位图只覆盖笔画包围盒那一小块，这是它左上角在窗口里的位置。
	// mosaicBrush 要按它平移，否则贴图会跑到窗口原点去。
	D2D1_POINT_2F mosaicOrigin{ 0.f, 0.f };
	float strokeWidth{ 1.f }, pressX{ 0.f }, pressY{ 0.f };
	bool isRect{ false };
};
