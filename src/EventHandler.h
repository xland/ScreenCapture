#pragma once
#include <functional>
#include "TypeDefine.h"

class EventHandler
{
public:
	EventHandler();
	~EventHandler();
protected:
	void listenMouseMove(const MouseEventCB&& func);
	void listenMouseDrag(const MouseEventCB&& func);
	void listenLeftBtnDown(const MouseEventCB&& func);
	void listenLeftBtnUp(const MouseEventCB&& func);
	void listenCustomMsg(const CustomEventCB&& func);
	void listenPaint(const PaintEventCB&& func);
private:

};