#pragma once
#include <include/Ling.h>

class ToolMain;
class ToolSub;
class ShapeBase;
class History;
class WinPin : public Ling::WinBase
{
public:
	~WinPin();
	static void init(int x, int y, int w, int h);
	void layoutTools();
public:
	std::unique_ptr<ToolMain> toolMain;
	std::unique_ptr<ToolSub> toolSub;
	ShapeBase* shapeHover{ nullptr };
	std::unique_ptr<History> history;
private:
	WinPin(int x, int y, int w, int h);
	void onCreated() override;
	void layout() override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
	void onDown(POINT pos, BOOL isRight);
	void onMove(POINT pos);
	void onUp(POINT pos, BOOL isRight);
	BOOL setCursor() override;
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg;
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
	bool isTopmost{ true }, isMouseDown{false};
	POINT pressPos{ 0,0 };
};

