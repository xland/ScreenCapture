#pragma once
#include <functional>
#include "TypeDefine.h"

class EventHandler
{
public:
	EventHandler();
	~EventHandler();
protected:
	void onMouseMove(const MouseEventCB& func);
	void onMouseDrag(const MouseEventCB& func);
	void onLeftBtnDown(const MouseEventCB& func);
	void onLeftBtnUp(const MouseEventCB& func);
	void onCustomMsg(const CustomEventCB& func);
private:

};