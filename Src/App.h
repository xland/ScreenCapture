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
	private:
		App();
};

