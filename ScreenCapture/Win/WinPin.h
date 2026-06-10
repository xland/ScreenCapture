#pragma once
#include "pch.h"
#include "WinBase.h"

class WinToolMain;
class WinToolSub;
class ShapeBase;
class History;
class WinPin :public WinBase
{
public:
	WinPin(const int& x, const int& y, const int& w, const int& h);
	~WinPin();
	static void init();
	static WinPin* getCur();
	void copyToClipboard();
	void saveToFile();
public:
	ShapeBase* shapeHover{ nullptr };
	std::unique_ptr<History> history;
	std::unique_ptr<WinToolMain> toolMain;
	std::unique_ptr<WinToolSub> toolSub;
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
	bool getImagePixels(std::vector<BYTE>& pixels);
	void initImg();
	void initTool();
private:
	POINT pressPos{0,0};
};

