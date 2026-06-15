#pragma once
#include "pch.h"
#include "WinBase.h"

class WinCutMask;
class ToolVideo;
class WinVideo:public WinBase
{
public:
	WinVideo(const int& x, const int& y, const int& w, const int& h);
	~WinVideo();
	static void init();
	static void release();
public:
	std::unique_ptr<WinCutMask> cutMask;
private:
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void makeTool();
private:
	std::unique_ptr<ToolVideo> tool;	
	bool isFinishCutMask{ false };
};

