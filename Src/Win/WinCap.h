#pragma once
#include <include/Ling.h>

class CutMask;
class WinCap:public Ling::WinBase
{
public:
	~WinCap();
	static void init();
	static WinCap* get();
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> getCutImg();
private:
	WinCap();
	void onCreated() override;
	void layout() override;
	BOOL setCursor() override;
	void setPixPos(POINT pos);
	void getPixImg(POINT pos);
	void paintPix(ID2D1DeviceContext* ctx);
	void onKey(UINT key);
	std::tuple<int, int, int, int> getCMYK(const BYTE& r, const BYTE& g, const BYTE& b);
private:
	std::unique_ptr<CutMask> cutMask;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg,pixImg;
	D2D1_RECT_F pixSrcRect{};
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	POINT pixPos;
	bool isPress{ false };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBg;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushText;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> crossBrush;
};

