#pragma once
#include <include/Ling.h>
#include <vector>
#include "OcrDoc.h"
class ImgViewer :public Ling::Node
{

public:
	ImgViewer(Ling::WinBase* win);
	~ImgViewer();
	void loadImg(const std::wstring& imgPath);
	// 选区由 TextBox 改动后，主窗口调这里重画高亮
	void redrawSelection() { paint(); }
	// 鼠标是否落在实际绘制出的位图矩形内（居中留白不算）
	bool isPosInImage(POINT pt) const;
	bool isSelecting() const { return selecting; }
	OcrDoc* getDoc() { return &doc; }
private:
	void paint();
	void layout() override;
	void readImg(const uint8_t* data, UINT w, UINT h);
	void getDrawParams(float& scale, float& dx, float& dy) const;
	void drawBitmap(ID2D1DeviceContext* ctx);
	void drawRects(ID2D1DeviceContext* ctx, POINT surfaceOffset);
	void onDown(POINT pt, bool isRight);
	void onMove(POINT pt);
	void onUp(POINT pt);
	// 窗口坐标 -> 原图像素坐标
	bool toImagePos(POINT pt, float& px, float& py) const;
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap> bitmap;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	// OCR 字符覆盖框用的半透明画刷（懒创建）
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> overlayBrush{ nullptr };
	OcrDoc doc;
	bool selecting{ false };
};

