#include "EventHandler.h"
#include "App.h"
#include "WinBase.h"

EventHandler::EventHandler()
{
}

EventHandler::~EventHandler()
{
}

void EventHandler::onMouseMove(const MouseEventCB& func)
{
	auto win = App::GetWin();
	win->mouseMoveHandlers.push_back(func);
}

void EventHandler::onCustomMsg(const CustomEventCB& func)
{
	auto win = App::GetWin();
	win->customEventHandlers.push_back(func);
}

void EventHandler::onLeftBtnDown(const MouseEventCB& func)
{
	auto win = App::GetWin();
	win->leftBtnDownHandlers.push_back(func);
}

void EventHandler::onLeftBtnUp(const MouseEventCB& func)
{
	auto win = App::GetWin();
	win->leftBtnUpHandlers.push_back(func);
}

void EventHandler::onMouseDrag(const MouseEventCB& func)
{
	auto win = App::GetWin();
	win->mouseDragHandlers.push_back(func);
}
