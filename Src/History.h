#pragma once
#include <include/Ling.h>
class ShapeBase;
class WinPin;
class History
{
public:
	History(WinPin* win);
	~History();
	ShapeBase* createShape(const std::wstring& state, const int& x, const int& y);
	void undo();
	void redo();
	void removeHoverShape();
	// 删掉指定 shape。ShapeText 输入为空时会异步调它把自己抹掉。
	void removeShape(ShapeBase* target);
public:
	std::vector<std::unique_ptr<ShapeBase>> shapes;
private:
	void removeUndoShape();
private:
	WinPin* win;
};

