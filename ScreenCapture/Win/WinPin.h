#pragma once
#include "pch.h"
#include "WinBase.h"

class ToolMain;
class ToolSub;
class ShapeBase;
class History;
enum class ToolPlacement
{
	BottomRight,        // ToolMain 位于 WinPin 右下方(下方)，ToolSub 出现在 ToolMain 下方
	TopRight,           // ToolMain 位于 WinPin 右上方(上方)，ToolSub 出现在 ToolMain 下方(WinPin 与 ToolMain 之间)
	OverlapBottomRight, // ToolMain 叠加在 WinPin 右下方内部，ToolSub 出现在 ToolMain 下方(同样叠加在 WinPin 内部)
};

class WinPin :public WinBase
{
public:
	WinPin(const int& x, const int& y, const int& w, const int& h);
	~WinPin();
	static void init();
	static void initFromData(int x, int y, int w, int h, std::vector<BYTE>& data);
	static WinPin* getCur();
	void copyToClipboard();
	void saveToFile();
	// 根据 WinPin 位置和所在显示器的可用空间，确定 ToolMain/ToolSub 的布局位置
	void updateToolLayout(bool subVisible);
	void bringTopmostToFront();
public:
	ShapeBase* shapeHover{ nullptr };
	ToolPlacement toolPlacement{ ToolPlacement::BottomRight };
	std::unique_ptr<History> history;
	std::unique_ptr<ToolMain> toolMain;
	std::unique_ptr<ToolSub> toolSub;
	ComPtr<ID2D1Bitmap1> screenImg;
private:
	void onCreated() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseDoubleClick(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onMouseWheel(const int& x, const int& y, const short& delta) override;
	void onPaint() override;
	BOOL onCursor() override;
	void onTimer(const UINT& timerId) override;
	void onKeyDown(const UINT& key) override;
	void onDestroy() override;
	void onDpiChanged() override;
	bool getImagePixels(std::vector<BYTE>& pixels);
	void initImg();
	void initImgFromData(std::vector<BYTE>& data);
	void initTool();
	void restoreWindowState(HWND foregroundBeforeDialog);
private:
	POINT pressPos{0,0};
	ComPtr<ID2D1SolidColorBrush> borderBrush;
};

