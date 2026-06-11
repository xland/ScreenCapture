#pragma once
class WinToolSub;
class WinToolSubSlider
{
public:
	WinToolSubSlider(WinToolSub* win);
	~WinToolSubSlider();
	void paint();
	void mouseWheel(const int& x, const int& y, const short& delta);
	void mouseDrag(const int& x, const int& y);
	void mouseMove(const int& x, const int& y);
	void mouseDown(const int& x, const int& y);
	void setVals(const float& max,const float& min,const float& val, const float& start, bool visible);
public:
	bool visible{ false };
	float start, end, val, min, max;
private:
	WinToolSub* win;
};

