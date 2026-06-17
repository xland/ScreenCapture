#pragma once
#include "pch.h"
#include "WinBase.h"
#include "WinVideoMp4.hpp"
#include "WinVideoGif.hpp"

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
	std::wstring stop();
public:
	std::unique_ptr<WinCutMask> cutMask;
private:
	LRESULT onHitTest(WPARAM wParam, LPARAM lParam) override;
	void onPaint() override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseUp(const int& x, const int& y) override;
	void onKeyDown(const UINT& key) override;
	void makeTool();
	void drawCursor(HDC hMemDC,const int& width,const int& height);
	void setMouseTransparent(bool transparent);
private:
	std::unique_ptr<ToolVideo> tool;	
	bool isFinishCutMask{ false }, isMouseTransparent{false};
	std::unique_ptr<WinVideoMp4::DESKTOPCAPTUREPARAMS> mp4Param;
	std::unique_ptr<WinVideoGif::GifParam> gifParam;
	std::jthread captureThread;
};

