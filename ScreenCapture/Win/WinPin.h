#pragma once
#include "pch.h"
#include "WinBase.h"

class ToolMain;
class ToolSub;
class ShapeBase;
class History;
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
public:
	ShapeBase* shapeHover{ nullptr };
	std::unique_ptr<History> history;
	std::unique_ptr<ToolMain> toolMain;
	std::unique_ptr<ToolSub> toolSub;
	ComPtr<ID2D1Bitmap1> screenImg;
private:
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
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
private:
	POINT pressPos{0,0};
};

