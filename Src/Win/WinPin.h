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
	void onTimerCB(UINT id);
	void onClosed();
	BOOL setCursor() override;
private:
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg;
	// 整个窗口内容都画在这块画布上，走 swap chain 后端：贴图窗口拖动 shape 时每帧重绘，
	// 单缓冲的合成表面会被采样到"擦干净→逐个重画"的中间态，表现为 shape 和边框整帧闪掉。
	Ling::Canvas* canvas{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
	bool isTopmost{ true }, isMouseDown{false}, isClosed{ false };
	POINT pressPos{ 0,0 };
};

