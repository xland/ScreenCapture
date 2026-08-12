#include "pch.h"
#include <CommCtrl.h>
#include "Tip.h"

Tip::Tip(Ling::WinBase* win) :win(win)
{
	// 整个进程只需要注册一次 tooltip 控件类
	static bool inited{ false };
	if (!inited) {
		INITCOMMONCONTROLSEX iccex{ sizeof(iccex), ICC_BAR_CLASSES };
		InitCommonControlsEx(&iccex);
		inited = true;
	}
	auto hIns = GetModuleHandle(nullptr);
	tipHwnd = CreateWindowEx(WS_EX_TOPMOST, TOOLTIPS_CLASS, nullptr, WS_POPUP | TTS_NOPREFIX | TTS_ALWAYSTIP,
		CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, win->hwnd, nullptr, hIns, nullptr);
	TOOLINFOW ti{};
	ti.cbSize = TTTOOLINFOW_V2_SIZE;
	// TTF_TRACK：不让控件自己跟着鼠标跑，显示时机和位置全由我们控制
	// TTF_ABSOLUTE：TTM_TRACKPOSITION 给的坐标就是提示窗口左上角，系统不再另行偏移
	ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
	ti.hwnd = win->hwnd;
	ti.hinst = hIns;
	ti.uId = 0;
	ti.lpszText = const_cast<LPWSTR>(L" ");
	SendMessage(tipHwnd, TTM_ADDTOOLW, 0, (LPARAM)&ti);
	// 延时由我们自己的定时器管，控件这边的初始延时清零
	SendMessage(tipHwnd, TTM_SETDELAYTIME, TTDT_INITIAL, MAKELONG(0, 0));
	timerTok = win->onTimer.add([this](UINT id) {
		if (id != timerId) return;
		this->win->killTimer(timerId);
		// 等待期间提示被取消（鼠标已经离开）时 owner 是空的
		if (!owner) return;
		showNow();
	});
}

Tip::~Tip()
{
	win->onTimer.remove(timerTok);
	if (owner) win->killTimer(timerId);
	if (tipHwnd && IsWindow(tipHwnd)) {
		DestroyWindow(tipHwnd);
	}
}

void Tip::bind(Ling::Button* btn, const std::wstring& text)
{
	// btn 挂在宿主窗口的节点树上，与 Tip 同生死（都随窗口销毁），捕获裸指针安全
	btn->onEnter.add([this, text](Ling::Button* b) { showAbove(b, text); });
	btn->onLeave.add([this](Ling::Button* b) { hide(b); });
}

void Tip::showAbove(Ling::Node* owner, const std::wstring& text)
{
	// Node 的 x/y/w/h 是布局算完的窗口内绝对坐标（物理像素），加上窗口位置就是屏幕坐标
	auto sx = win->x + owner->x + owner->w / 2.f;
	auto sy = win->y + owner->y + anchorInset * win->dpi;
	showAt(owner, sx, sy, text);
}

void Tip::showAt(Ling::Node* owner, float screenX, float screenY, const std::wstring& text)
{
	if (this->owner == owner) {
		// 同一个归属：只更新内容与位置。已经显示出来的就立刻跟上（滑块提示跟着鼠标走靠这条），
		// 还在等那 1 秒的则保持计时不重置，到点后按最新位置显示。
		this->text = text;
		anchorX = screenX;
		anchorY = screenY;
		if (visible) showNow();
		return;
	}
	// 换归属：旧提示立刻收掉，1 秒重新计时
	hide();
	this->owner = owner;
	this->text = text;
	anchorX = screenX;
	anchorY = screenY;
	win->setTimer(delayMs, timerId);
}

void Tip::hide(Ling::Node* owner)
{
	if (this->owner == owner) hide();
}

void Tip::hide()
{
	if (owner) {
		win->killTimer(timerId);
		owner = nullptr;
	}
	text.clear();
	if (!visible) return;
	visible = false;
	TOOLINFOW ti{};
	ti.cbSize = TTTOOLINFOW_V2_SIZE;
	ti.hwnd = win->hwnd;
	ti.uId = 0;
	SendMessage(tipHwnd, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
	SendMessage(tipHwnd, TTM_ACTIVATE, FALSE, 0);
}

void Tip::showNow()
{
	TOOLINFOW ti{};
	ti.cbSize = TTTOOLINFOW_V2_SIZE;
	ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
	ti.hwnd = win->hwnd;
	ti.uId = 0;
	ti.lpszText = const_cast<LPWSTR>(text.c_str());
	SendMessage(tipHwnd, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
	// 气泡没显示出来也能先问尺寸，于是一次就能摆到准确位置，
	// 不用再“先显示再挠”（那一下看着就像提示抖了一下）
	auto bubble = SendMessage(tipHwnd, TTM_GETBUBBLESIZE, 0, (LPARAM)&ti);
	// 底边中点对齐到锚点：横向居中，纵向整个提示都在锚点上方
	auto px = static_cast<int>(anchorX) - LOWORD(bubble) / 2;
	auto py = static_cast<int>(anchorY) - HIWORD(bubble);
	SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(px, py));
	if (!visible) {
		SendMessage(tipHwnd, TTM_ACTIVATE, TRUE, 0);
		SendMessage(tipHwnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
		visible = true;
		// 激活那一下系统可能按自己那套摆过，再定一遍位保险
		SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(px, py));
	}
	// 提上最顶层：建窗时的 WS_EX_TOPMOST 只保证它在置顶那一群里，
	// 而工具窗口个个都是置顶（ToolSub 的提示正好落在 ToolMain 上），
	// 不每次重新抢到同组最前面就会被它们盖住。SWP_NOACTIVATE：别抢焦点。
	SetWindowPos(tipHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
