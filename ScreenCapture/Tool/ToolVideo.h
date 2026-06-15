#pragma once
#include "pch.h"
#include "ToolBase.h"

class WinVideo;
class ToolVideo :public ToolBase
{
public:
	ToolVideo(const int& x, const int& y, const int& w, const int& h, WinVideo* parent);
	~ToolVideo();
public:
	std::string state;
protected:
private:
	void onPaint() override;
	BOOL onCursor() override;
	void onMouseDown(const int& x, const int& y, bool isRight) override;
	void onMouseMove(const int& x, const int& y) override;
	void onMouseLeave() override;
	void onDpiChanged() override;
	void onCreated() override;
	bool isBlueForeColor(const int& index);
private:
	WinVideo* parent;
	ComPtr<IDWriteTextLayout> formatLable,formatMp4,formatGif;
	float mp4Start,mp4End, gifEnd, borderRadius{ 3.f }, paddingTopBottom{ 3.f },speakerStart,micStart, clapperStart,closeStart;
	int hoverIndex{ -1 }, selectIndexFormat{ 0 };
	bool selectSpeaker{ true }, selectMic{ false };
};

