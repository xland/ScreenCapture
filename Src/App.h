#pragma once
#include <include/Ling.h>

class App
{
	public:
		~App();
		static void init();
		// 消息循环退出后、Ling::dispose 之前调用：单例里存着 WinRT 对象（配置和语言都是
		// JsonObject），等到进程退出后的静态析构时 CoUninitialize 早跑完了，
		// com_ptr 析构里那句 Release 打到的是已经拆掉的对象 —— 表现为读取访问权限冲突
		static void dispose();
		static App* get();
		void takeScreenShot(int x, int y, int w, int h,ID2D1Bitmap1** img);
		std::tuple<int, int, int, int> getScreenArea();
		// 活干完了（回到只剩托盘图标的状态）：延时把显卡那边的缓存还回去。
		// Ling 自己判断不出这个时机 —— 它不跟踪活动窗口，也不知道"托盘常驻"这回事
		static void trimMemoryLater();
	private:
		App();
};

