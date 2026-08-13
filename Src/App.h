#pragma once
#include <include/Ling.h>

class App
{
	public:
		~App();
		static void init();
		static App* get();
		void takeScreenShot(int x, int y, int w, int h,ID2D1Bitmap1** img);
		std::tuple<int, int, int, int> getScreenArea();
		// 活干完了（回到只剩托盘图标的状态）：延时把显卡那边的缓存还回去。
		// Ling 自己判断不出这个时机 —— 它不跟踪活动窗口，也不知道"托盘常驻"这回事
		static void trimMemoryLater();
	private:
		App();
};

