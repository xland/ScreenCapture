#pragma once
#include <include/Ling.h>

class App
{
	public:
		~App();
		static void init();
		static App* get();
		// 用完即走模式。老版本传的是裸的 auto-quit，新的习惯带 -- 前缀，
		// 而 Ling 的 initArgs 是把参数原样当 key 入表的，所以按子串认
		static bool isAutoQuit();
		void takeScreenShot(int x, int y, int w, int h,ID2D1Bitmap1** img);
		std::tuple<int, int, int, int> getScreenArea();
	private:
		App();
};

