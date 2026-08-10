#pragma once
#include <include/Ling.h>
#include "ShapeBase.h"
class ShapeEraser : public ShapeBase
{
public:
	ShapeEraser(WinPin* win);
	~ShapeEraser();
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
	void resetEraser();
	void updateDraggers();
	void initBackgroundBrush();
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
	// 拿窗口底图做的画刷。"擦除"不是真的擦掉，是用原始底图盖回去 ——
	// 底图与窗口同尺寸同坐标，所以画刷不需要平移就能对齐
	Microsoft::WRL::ComPtr<ID2D1BitmapBrush> bgBrush;
	float strokeWidth{ 1.f }, pressX{ 0.f }, pressY{ 0.f };
	// isErasing 要等几何定下来（mouseUp）才置位：之前显示占位色，之后才真正盖底图
	bool isRect{ false }, isErasing{ false };
};
