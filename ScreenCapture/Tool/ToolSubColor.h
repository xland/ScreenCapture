#pragma once
class ToolSub;
class ToolSubColor
{
public:
	ToolSubColor(ToolSub* win);
	~ToolSubColor();
	void paint();
	void winReady();
	bool hover(const int& x);
	bool mouseDown(const int& x, const int& y);
	void setVals(const float& start,bool visible);
	D2D1_COLOR_F getSelectedColor();
public:
	bool visible{ false };
	float start, end;
	int indexSelected{ 0 }, indexHovered{-1};
private:
	void createOneBrush(const int& color);
private:
	ToolSub* win;
	std::vector<ComPtr<ID2D1SolidColorBrush>> brushes;
	std::vector<std::wstring> names;
	float btnSize;
};

