#pragma once
#include <include/Ling.h>

class CutMask;
class ToolLong;
class WinLong : public Ling::WinBase
{
public:
	~WinLong();
	static void init();
	void copyToClipboard();
	void saveToFile();
	void pin();
public:
	std::unique_ptr<CutMask> cutMask;
private:
	WinLong();
	void onCreated() override;
	void layout() override;
	BOOL setCursor() override;
	void onDown(POINT pos, bool isRight);
	void onMove(POINT pos);
	void onUp(POINT pos, bool isRight);
	void onTimerCB(UINT timerId);
	void onKey(UINT key);
	void onClosed();
	void firstStep();
	void makeImgPreview();
	void capStep();
	void hollowWin();
	void makeTool();
	void paintImgPreview(ID2D1DeviceContext* ctx);
	void stopCap();
	void makeStopText();
private:
	bool isFinishCutMask{ false }, isShowStartBtn{ false }, isScrolling{ false }, isFinish{ false };
	// Ling 没有 onMouseDrag，靠这个标志在 onMove 里区分"拖框"和"纯移动"
	bool isMouseDown{ false }, isClosed{ false };
	bool firstCheck{ true };
	int dismissTime{ 0 };
	int changeStartY{ -1 };
	D2D1_RECT_F stopTextRect{};
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> bgBrush;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> layoutTextStart;
	Microsoft::WRL::ComPtr<IDWriteTextLayout> layoutTextEnd;
	float startCircleR{ 30.f };
	POINT circleCenter{};
	HWND targetHwnd{ nullptr };
	std::unique_ptr<ToolLong> tool;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> imgPreview;
	std::vector<BYTE> imgData;
	std::vector<BYTE> img1;
	int imgW{ 0 }, imgH{ 0 };
	int resultH{ 0 };
	POINT capStartPos{};
	// 内容全画在这块画布上，走 swap chain 后端：滚动时预览图每帧重画，单缓冲会闪
	Ling::Canvas* canvas{ nullptr };
};
