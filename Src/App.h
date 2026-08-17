#pragma once
#include <include/Ling.h>

class App
{
	public:
		~App();
		static void init();
		// 消息循环退出后、Ling::dispose 之前调用：单例和窗口对象里存着 WinRT / D2D 对象
		//（配置和语言是 JsonObject，贴图窗口攥着位图和画刷），等到进程退出后的静态析构时
		// CoUninitialize 早跑完了，析构里那句 Release 打到的是已经拆掉的对象 ——
		// 表现为读取访问权限冲突
		static void dispose();
		static App* get();
		void takeScreenShot(int x, int y, int w, int h,ID2D1Bitmap1** img);
		std::tuple<int, int, int, int> getScreenArea();
		// 把窗口从屏幕捕获里摘出去：它在屏幕上照常显示、照常能点，但录屏和抓屏都拿不到它。
		// 录屏工具条压在录制区内部时（全屏录制必然如此）靠这个才不会被录进去。
		// MP4 走的 DXGI 桌面复制和 GIF 走的 BitBlt 抓屏都认这个标记（后者实测验证过）。
		// 需要 Windows 10 2004（build 19041）以上，更老的系统上调用直接失败，此时维持原样
		//（照旧录进去）—— 退回 WDA_MONITOR 会在录像里留一块纯黑，比录到工具条更难解释
		static void excludeFromCapture(HWND hwnd);
	private:
		App();
};

