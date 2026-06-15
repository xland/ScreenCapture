#pragma once
#include "pch.h"
#include "WinBase.h"
#include "WinVideoMp4.hpp"

class WinCutMask;
class ToolVideo;
class WinVideo:public WinBase
{
public:
	WinVideo(const int& x, const int& y, const int& w, const int& h);
	~WinVideo();
	static void init();
	static void release();
	void startMp4(bool useSpeaker, bool useMic);
	void startGif();
	void stop();
public:
	std::unique_ptr<WinCutMask> cutMask;
private:
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void makeTool();
	void drawCursor(HDC hMemDC);
private:
	std::unique_ptr<ToolVideo> tool;	
	bool isFinishCutMask{ false }, isFinishGifRecord{false};
	WinVideoMp4::DESKTOPCAPTUREPARAMS dp;
	std::jthread captureThread;
};

