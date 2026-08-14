#include "pch.h"
#include <CommCtrl.h>
#include "Tip.h"
#include "App.h"

namespace {
	// 气泡窗口尺寸减去文字尺寸剩下的那圈（边框 + 内边距）。
	// 它只跟主题、边框有关，跟文字无关，整个进程量一次就够。-1 表示还没量过
	SIZE tipPadding{ -1, -1 };
}

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
	learnPadding();
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

void Tip::excludeFromCapture()
{
	App::excludeFromCapture(tipHwnd);
}

SIZE Tip::measureText(const std::wstring& str)
{
	RECT rc{};
	auto dc = GetDC(tipHwnd);
	auto font = (HFONT)SendMessage(tipHwnd, WM_GETFONT, 0, 0);
	auto oldFont = font ? (HFONT)SelectObject(dc, font) : nullptr;
	DrawTextW(dc, str.c_str(), (int)str.size(), &rc, DT_CALCRECT | DT_SINGLELINE | DT_NOPREFIX);
	if (oldFont) SelectObject(dc, oldFont);
	ReleaseDC(tipHwnd, dc);
	return SIZE{ rc.right - rc.left, rc.bottom - rc.top };
}

// 提示显示出来之前问不到气泡尺寸（TTM_GETBUBBLESIZE 在那之前会在 comctl32 里空指针崩），
// 而先显示再校正位置会被系统的淡入动画拘成一次看得见的滑动。
// 所以这里偷偷显示一次：整窗 alpha 0 全透明，量完内边距立刻收掉，屏上什么也看不见。
void Tip::learnPadding()
{
	if (tipPadding.cx >= 0) return;
	auto exStyle = GetWindowLongPtr(tipHwnd, GWL_EXSTYLE);
	SetWindowLongPtr(tipHwnd, GWL_EXSTYLE, exStyle | WS_EX_LAYERED);
	SetLayeredWindowAttributes(tipHwnd, 0, 0, LWA_ALPHA);
	static constexpr auto probe = L"Ag";
	TOOLINFOW ti{};
	ti.cbSize = TTTOOLINFOW_V2_SIZE;
	ti.uFlags = TTF_TRACK | TTF_ABSOLUTE;
	ti.hwnd = win->hwnd;
	ti.uId = 0;
	ti.lpszText = const_cast<LPWSTR>(probe);
	SendMessage(tipHwnd, TTM_UPDATETIPTEXTW, 0, (LPARAM)&ti);
	SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(0, 0));
	SendMessage(tipHwnd, TTM_ACTIVATE, TRUE, 0);
	SendMessage(tipHwnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
	RECT rc{};
	GetWindowRect(tipHwnd, &rc);
	SendMessage(tipHwnd, TTM_TRACKACTIVATE, FALSE, (LPARAM)&ti);
	SendMessage(tipHwnd, TTM_ACTIVATE, FALSE, 0);
	SetLayeredWindowAttributes(tipHwnd, 0, 255, LWA_ALPHA);
	SetWindowLongPtr(tipHwnd, GWL_EXSTYLE, exStyle);
	auto textSize = measureText(probe);
	tipPadding = SIZE{ rc.right - rc.left - textSize.cx, rc.bottom - rc.top - textSize.cy };
}

SIZE Tip::measure()
{
	auto size = measureText(text);
	if (tipPadding.cx >= 0) {
		size.cx += tipPadding.cx;
		size.cy += tipPadding.cy;
		return size;
	}
	// 内边距没量到（不应该发生）：退回让控件自己撑，差的那点由显示后的校正兜着
	RECT rc{ 0, 0, size.cx, size.cy };
	SendMessage(tipHwnd, TTM_ADJUSTRECT, TRUE, (LPARAM)&rc);
	return SIZE{ rc.right - rc.left, rc.bottom - rc.top };
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
	// 先按自己量的尺寸摆到位，再激活，省掉“先显示再挠”那一下拖动
	auto size = measure();
	auto px = static_cast<int>(anchorX) - size.cx / 2;
	auto py = static_cast<int>(anchorY) - size.cy;
	SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(px, py));
	if (!visible) {
		SendMessage(tipHwnd, TTM_ACTIVATE, TRUE, 0);
		SendMessage(tipHwnd, TTM_TRACKACTIVATE, TRUE, (LPARAM)&ti);
		visible = true;
		// 激活那一下系统可能按自己那套摆过，再定一遍位保险
		SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(px, py));
	}
	// 自己量的和控件实际用的尺寸可能差一两像素，显示出来后按真实尺寸校正一次
	RECT rc{};
	GetWindowRect(tipHwnd, &rc);
	auto realX = static_cast<int>(anchorX) - (rc.right - rc.left) / 2;
	auto realY = static_cast<int>(anchorY) - (rc.bottom - rc.top);
	if (realX != px || realY != py) {
		SendMessage(tipHwnd, TTM_TRACKPOSITION, 0, MAKELPARAM(realX, realY));
	}
	// 提上最顶层：建窗时的 WS_EX_TOPMOST 只保证它在置顶那一群里，
	// 而工具窗口个个都是置顶（ToolSub 的提示正好落在 ToolMain 上），
	// 不每次重新抢到同组最前面就会被它们盖住。SWP_NOACTIVATE：别抢焦点。
	SetWindowPos(tipHwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
}
