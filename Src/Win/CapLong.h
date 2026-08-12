#pragma once
#include <include/Ling.h>

class WinCap;
class ToolLong;
// 滚动截图。选区已经由 WinCap 定好了，所以它不是窗口，画布、定时器、光标全借宿主的。
class CapLong
{
public:
	CapLong(WinCap* win);
	~CapLong();
	// 宿主窗口要销毁了：停掉定时器、收掉工具条
	void dispose();
	// 以下几个都由 WinCap 在对应的事件里转进来
	void onMove(POINT pos);
	void onUp(POINT pos);
	void onTimerCB(UINT timerId);
	void setCursor();
	void paint(ID2D1DeviceContext* ctx);
	// 三个都是 ToolLong 的按钮动作，做完由 ToolLong 关掉宿主窗口
	void copyToClipboard();
	void saveToFile();
	void pin();
private:
	void firstStep();
	void makeImgPreview();
	void capStep();
	void makeTool();
	void paintImgPreview(ID2D1DeviceContext* ctx);
	void stopCap();
	void makeStopText();
private:
	WinCap* win;
	bool isShowStartBtn{ false }, isScrolling{ false }, isFinish{ false };
	bool firstCheck{ true };
	int dismissTime{ 0 };
	int changeStartY{ -1 };
	D2D1_RECT_F stopTextRect{};
	// 两处文字的绘制起点。IDWriteTextLayout 默认左上对齐，DrawTextLayout 给的又是
	// layout 框的左上角，所以得先测出文本实际宽高，才能算出居中要的那个起点
	D2D1_POINT_2F stopTextPos{};
	D2D1_SIZE_F startTextSize{};
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
};
