#pragma once
#include <include/Ling.h>
#include "OcrDoc.h"
class TextBox :public Ling::Node
{
public:
	TextBox(Ling::WinBase* win);
	~TextBox();
	// doc 由 ImgViewer 持有，这里只借用（生命周期覆盖本控件）
	void loadText(OcrDoc* doc);
	// 选区由 ImgViewer 改动后，主窗口调这里重画高亮
	void redrawSelection() { paint(); }
	// pt（窗口坐标）是否落在文本内容区内
	bool isPosInContent(POINT pt) const;
	bool isSelecting() const { return selecting; }
	// 让选区末端在右侧列表里可见（图像侧拖拽时联动用）
	void scrollFocusIntoView();
private:
	void paint();
	void layout() override;
	void onDown(POINT pt, bool isRight);
	void onMove(POINT pt);
	void onUp(POINT pt);
	// 重算每行 layout 的位置与本控件总高度。宽度变化后必须调用，
	// 否则换行数变了而 linePoss 还是旧的，文字会重叠。
	// 返回 true 表示总高度变了，需要再来一次布局。
	bool relayoutLines();
	// 窗口坐标 -> 本控件内容坐标（已计入 ScrollerBox 的滚动偏移）
	POINT toLocal(POINT pt) const;
	// 内容坐标 -> 文本流位置
	TextPos hitTestLocal(POINT local) const;
	void drawSelection(ID2D1DeviceContext* ctx);
	// 拖拽到可视区上/下边缘外时自动滚动；返回是否滚动了
	bool autoScroll(POINT pt);
	void onTimer(UINT id);
	Ling::ScrollerBox* getScroller() const;
private:
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> selBrush{ nullptr };
	winrt::Windows::UI::Composition::CompositionDrawingSurface surface{ nullptr };
	std::vector<Microsoft::WRL::ComPtr<IDWriteTextLayout>> lineLayouts;
	std::vector<D2D1_POINT_2F> linePoss;
	OcrDoc* doc{ nullptr };
	bool selecting{ false };
	bool autoScrolling{ false };
	POINT lastDragPt{};
};

