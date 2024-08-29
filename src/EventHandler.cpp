#include "EventHandler.h"
#include "App.h"
#include "WinBase.h"

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

void EventHandler::listenMouseMove(const MouseEventCB&& func)
{
	auto win = App::GetWin();
	win->mouseMoveHandlers.push_back(func);
}

void EventHandler::listenCustomMsg(const CustomEventCB&& func)
{
	auto win = App::GetWin();
	win->customEventHandlers.push_back(func);
}

void EventHandler::listenPaint(const PaintEventCB&& func, const int& index)
{
	auto win = App::GetWin();
	if (index == -1) {
		win->paintHandlers.push_back(func);
	}
	else {
		win->paintHandlers.insert(win->paintHandlers.begin() + index, func);
	}
}

void EventHandler::listenLeftBtnDown(const MouseEventCB&& func)
{
	auto win = App::GetWin();
	win->leftBtnDownHandlers.push_back(func);
}

void EventHandler::listenLeftBtnUp(const MouseEventCB&& func)
{
	auto win = App::GetWin();
	win->leftBtnUpHandlers.push_back(func);
}

void EventHandler::listenMouseDrag(const MouseEventCB&& func)
{
	auto win = App::GetWin();
	win->mouseDragHandlers.push_back(func);
}
