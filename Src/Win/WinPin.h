#pragma once
#include <include/Ling.h>

class ToolMain;
class ToolSub;
class ShapeBase;
class ShapeText;
class History;
class WinPin : public Ling::WinBase
{
public:
	~WinPin();
	static void init(int x, int y, int w, int h);
	// 底图不来自 WinCap 的截屏，而是外部给的一块 BGRA、top-down、行紧凑（步长 = w*4）像素。
	// 滚动截图（WinLong）拼出来的长图走这条路进贴图窗口。
	static void initFromData(int x, int y, int w, int h, std::vector<BYTE>& data);
	void layoutTools();
	// 把底图与所有未撤销的 shape 合成后写入剪切板，成功即关窗
	void copyToClipboard();
	// 弹另存为对话框，把合成结果存成 PNG，成功即关窗；用户取消或失败则保持窗口
	void saveToFile();
	// 所有 ShapeText 共用的文本输入框，第一次用到时才建。
	// 共用而不是一个 shape 一个：TextBox 构造时会往窗口的十来个事件上挂回调，
	// N 个实例意味着每次鼠标移动都要跑 N 遍，而同一时刻只可能有一个 ShapeText 在编辑。
	Ling::TextBox* getTextBox();
	// ShapeText 进入 / 退出编辑时登记自己。传 nullptr 表示没有在编辑的文本。
	void setEditingText(ShapeText* shape);
	// ToolSub 上的颜色 / 字号 / 粗体 / 斜体变了，转给正在编辑的文本立即生效
	void onToolStyleChanged();
public:
	std::unique_ptr<ToolMain> toolMain;
	std::unique_ptr<ToolSub> toolSub;
	ShapeBase* shapeHover{ nullptr };
	std::unique_ptr<History> history;
	// 贴图窗口的底图。ShapeMosaic 要读它算马赛克块，ShapeEraser 拿它当"擦回原样"的画刷
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg;
private:
	WinPin(int x, int y, int w, int h, const std::vector<BYTE>* data = nullptr);
	void onCreated() override;
	void layout() override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
	void onDown(POINT pos, BOOL isRight);
	void onMove(POINT pos);
	void onUp(POINT pos, BOOL isRight);
	void onKey(UINT key);
	void onTimerCB(UINT id);
	void onClosed();
	BOOL setCursor() override;
	// 离屏合成出最终图像的像素（BGRA、top-down、行步长紧凑为 w*4）。
	// 只画底图和未撤销的 shape，不含蓝色边框和夹点。
	bool getImagePixels(std::vector<BYTE>& pixels);
	// 另存为对话框会抢走前台并把 WinPin 激活，取消保存后用它把窗口层级和前台窗口恢复原样
	void restoreWindowState(HWND foregroundBeforeDialog);
private:
	// 整个窗口内容都画在这块画布上，走 swap chain 后端：贴图窗口拖动 shape 时每帧重绘，
	// 单缓冲的合成表面会被采样到"擦干净→逐个重画"的中间态，表现为 shape 和边框整帧闪掉。
	Ling::Canvas* canvas{ nullptr };
	// 文本输入框与当前正在编辑的 ShapeText。非空表示"编辑中"：此时落在文本框里的
	// 鼠标事件、以及所有键盘事件都归 TextBox，WinPin 自己的那套要让路。
	Ling::TextBox* textBox{ nullptr };
	ShapeText* editingText{ nullptr };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> borderBrush;
	bool isTopmost{ true }, isMouseDown{false}, isClosed{ false };
	POINT pressPos{ 0,0 };
};

