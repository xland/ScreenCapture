#pragma once
#include "EventHandler.h"
class Recorder : public EventHandler
{
public:
	Recorder();
	~Recorder();
	void Init();
	void OnLeftBtnDown(const int& x, const int& y);
	void OnCustomMsg(const EventType& type, const uint32_t& msg);
	void OnMouseMove(const int& x, const int& y);
	void OnPaint(SkCanvas* canvas);
private:

};