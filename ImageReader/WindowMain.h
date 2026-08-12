#pragma once
#include <include/Ling.h>
class TitleBar;
class StatusBar;
class TextBox;
class ImgViewer;
class WindowMain : public Ling::WinBase
{
public:
	~WindowMain();
	static void init();
	static WindowMain* get();
public:
	ImgViewer* imgViewer;
	TextBox* textBox;
	Ling::ScrollerBox* scrollerBox;
private:
	WindowMain();
	void onCreated() override;
	LRESULT onHitTest(const POINT pos) override;
	void onSetCursor(bool* flag);
	void onDown(POINT pt, bool isRight);
	void onMove(POINT pt);
	void onUp(POINT pt);
	void onKey(UINT vk);
	// 选区变化：两侧各自重画自己的 surface。不走 refresh()，
	// 避免拖拽每一帧都触发一次完整 yoga 布局。
	void onSelectionChanged();
private:
	TitleBar* titleBar;
	StatusBar* statusBar;
	Ling::Node* splitter;
	bool isDragging{ false };
};

