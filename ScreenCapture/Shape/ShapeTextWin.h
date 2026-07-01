#pragma once
#include "pch.h"
#include "Win/WinBase.h"
class ShapeText;
class ShapeTextWin : public WinBase
{
public:
	ShapeTextWin(const int& x, const int& y, const int& w, const int& h);
	~ShapeTextWin();
	static ShapeTextWin* get();
	void setShape(ShapeText* shape);
	void changeState();
public:
protected:
private:
	void onCreated() override;
	void onPaint() override;
	void onMouseDrag(const int& x, const int& y, const UINT_PTR& modifiers) override;
	BOOL onCursor() override;
	void onMouseDown(const int& x, const int& y, bool isRight);
	void onMouseUp(const int& x, const int& y);
	void onTimer(const UINT& timerId) override;
	LRESULT onMouseActivate() override;
	void onKeyDown(const UINT& key) override;
	void onChar(const UINT& code) override;
	void onIME() override;
	void onBlur() override;
	void onShown() override;
	void onHidden() override;
	/// <summary>
	/// 通过鼠标坐标设置输入光标的位置
	/// </summary>
	/// <param name="x"></param>
	/// <param name="y"></param>
	void setCaretByMousePos(const float& x, const float& y);
	/// <summary>
	/// 从输入位置index推算输入光标坐标（输入光标所在的文本的位置->输入光标的坐标）
	/// </summary>
	/// <param name="textIndex"></param>
	void resetCaretPos(const int& textIndex);
	/// <summary>
	/// 绘制选中的文本的背景
	/// </summary>
	void paintSelectionBg();
	/// <summary>
	/// 绘制单行选中文本的背景
	/// </summary>
	/// <param name="start"></param>
	/// <param name="end"></param>
	void paintSelectionBgOneRown(const int& start, const int& end);
	/// <summary>
	/// 删除选中的文本
	/// </summary>
	/// <returns></returns>
	bool delSelection();
private:
	/// <summary>
	/// 形状对象指针，用于访问文本内容和布局等信息
	/// </summary>
	ShapeText* shape{ nullptr };
	/// <summary>
	/// 光标是否可见，用于实现闪烁效果
	/// </summary>
	bool caretVisible{ false };
	// 光标顶部坐标，相对于文本布局的左上角
	D2D1_POINT_2F caretPos;
	// 光标高度
	float caretHeight{ 0.f };
	/// <summary>
	/// 选中文本的背景画刷
	/// </summary>
	ComPtr<ID2D1SolidColorBrush> textSelectionBgBrush;
	/// <summary>
	/// 选中文本的起始位置
	/// </summary>
	int caretSelectionStart{ 0 };
	/// <summary>
	/// 选中文本的结束位置
	/// </summary>
	int caretSelectionEnd{ 0 };
	/// <summary>
	/// 选中文本动作，鼠标按下的位置
	/// </summary>
	int caretSelectionDown{ 0 };
};

