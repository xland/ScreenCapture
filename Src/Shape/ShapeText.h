#pragma once
#include <include/Ling.h>
#include "ShapeBase.h"

// 屏幕文字。
//
// 编辑态与非编辑态用的是两套东西：
//   编辑中  —— 文本由 WinPin 上那个共用的 Ling::TextBox 画（它有自己的 swapchain，
//              光标、选区、IME 都归它），本 shape 只负责那圈虚线框；
//   编辑完 —— TextBox 隐藏，文本由本 shape 自己的 textLayout 画进 WinPin 的画布，
//              这样它才会出现在保存/复制出去的图里（导出走的是离屏 paint(ctx)）。
class ShapeText : public ShapeBase
{
public:
	ShapeText(WinPin* win);
	~ShapeText();
	void paint(ID2D1DeviceContext* ctx) override;
	void paintDragger(ID2D1DeviceContext* ctx) override;
	void mouseDrag(const float x, const float y) override;
	void mouseDown(const float x, const float y) override;
	void mouseMove(const float x, const float y) override;
	void setCursor() override;
	// 单击进编辑态，留不留由 finishEdit 按文本是否为空决定，这里不能提前删
	bool isValidWithoutDrag() override { return true; };
	// 收尾：把 TextBox 里的文字取回来自己画，空文本则把自己从 history 里删掉。
	// 除了本类内部，WinPin（导出图片前）和 History（删除 shape 前）也会调。
	void finishEdit();
	// ToolSub 上的颜色/字号/粗斜体变了，编辑中的话立即生效
	void applyStyle();
public:
	bool isEditing{ false };
private:
	void startEdit();
	void makeTextLayout();
	// 从 ToolSub 拉一份当前样式，并重建画刷
	void setAttr();
private:
	std::wstring text;
	// 物理像素。ToolSub::getSliderVal() 给的就是物理值，而 TextBox::setFontSize 收逻辑值，
	// 传过去时要除回 dpi。
	float fontSize{ 20.f };
	bool isBold{ false }, isItalic{ false };
	UINT32 colorValue{ 0 };
	D2D1_COLOR_F color{};
	// 边框盒，窗口客户区坐标、物理像素。文字画在它内缩 borderPadding 的位置。
	D2D1_RECT_F rect{};
	Microsoft::WRL::ComPtr<IDWriteTextLayout> textLayout;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> textBrush;
	Microsoft::WRL::ComPtr<ID2D1StrokeStyle> dashedStrokeStyle;
	// 文字到虚线框的间距。取 TextBox 默认的 setPadding(6.f)，两边画出来才对得上
	float borderPadding;
	float pressX{ 0.f }, pressY{ 0.f };
	// 编辑期间挂在 TextBox 上的两个订阅。TextBox 是共用的，退出编辑必须摘掉，
	// 否则下一个 ShapeText 编辑时会把文字写进已经结束的那个里。
	winrt::event_token textChangedTok{}, focusTok{};
};

