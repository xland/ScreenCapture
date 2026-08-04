#pragma once
#include <include/Ling.h>

class WinCap:public Ling::WinBase
{
public:
	~WinCap();
	static void init();
private:
	WinCap();
	void initPosSize();
	void onCreated() override;
	void layout() override;
	void initWinRects();
	BOOL setCursor() override;
	void setPixPos(POINT pos);
	void getPixImg(POINT pos);
	void dragMaskRect(POINT pos);
	void switchWinRect(POINT pos);
	void paintMaskRect(ID2D1DeviceContext* ctx);
	void paintPix(ID2D1DeviceContext* ctx);
	void onKey(UINT key);
	std::tuple<int, int, int, int> getCMYK(const BYTE& r, const BYTE& g, const BYTE& b);
private:
	D2D1_RECT_F maskRect,pixImgRect;
	std::vector<D2D1_RECT_F> winRect;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg,pixImg;
	D2D1_RECT_F pixSrcRect{};
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	POINT pressPos,pixPos;
	bool isPress{ false };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBg;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBorder;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushText;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> crossBrush;
};

