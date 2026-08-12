#pragma once
#include <include/Ling.h>
#include <d2d1_1.h>
#include <wrl.h>
#include <functional>
#include <string>
#include <vector>

namespace tinyocr { struct Result; }

// 原图坐标系下的一个四点框，点序 tl, tr, br, bl。
struct Quad {
	D2D1_POINT_2F p[4]{};
	bool valid{ false };
	float left()   const { return (std::min)((std::min)(p[0].x, p[1].x), (std::min)(p[2].x, p[3].x)); }
	float right()  const { return (std::max)((std::max)(p[0].x, p[1].x), (std::max)(p[2].x, p[3].x)); }
	float top()    const { return (std::min)((std::min)(p[0].y, p[1].y), (std::min)(p[2].y, p[3].y)); }
	float bottom() const { return (std::max)((std::max)(p[0].y, p[1].y), (std::max)(p[2].y, p[3].y)); }
};

// 文本流中的一个插入点。ch 取值 [0, lines[line].text.size()]，
// 等于 size() 表示行尾。按 (line, ch) 字典序即阅读顺序。
struct TextPos {
	int line{ 0 };
	int ch{ 0 };
	bool operator==(const TextPos& o) const { return line == o.line && ch == o.ch; }
	bool operator<(const TextPos& o) const { return line != o.line ? line < o.line : ch < o.ch; }
	bool operator<=(const TextPos& o) const { return !(o < *this); }
};

struct OcrLine {
	std::wstring text;
	// 与 text 逐字符对齐：charQuads[i] 是第 i 个字符在原图上的框。
	// 空格、以及 OCR 没给出几何的字符，valid=false。
	std::vector<Quad> charQuads;
	// 与 charQuads 一一对应的预建路径几何（valid=false 处为 nullptr）。
	// 在 loadResult 时建一次，拖拽期间只是组装 GeometryGroup，不重复建几何。
	std::vector<Microsoft::WRL::ComPtr<ID2D1PathGeometry>> charGeoms;
	// 本行所有 valid 字符框的并集 AABB（原图坐标）
	float boxLeft{ 0 }, boxTop{ 0 }, boxRight{ 0 }, boxBottom{ 0 };
	bool hasGeom{ false };
};

// OCR 结果 + 选区状态。ImgViewer 和 TextBox 共享同一份，
// 双向选择就是两侧各自把这份「字符区间」渲染成自己坐标系里的高亮。
class OcrDoc
{
public:
	// 吃下 OCR 结果：做 word box -> 字符下标的对齐，并预建每字符的路径几何。
	void loadResult(const tinyocr::Result& result);
	void clear();

	const std::vector<OcrLine>& getLines() const { return lines; }
	bool empty() const { return lines.empty(); }

	// ---- 选区 ----
	void setAnchor(const TextPos& pos) { anchor = pos; focus = pos; }
	void setFocus(const TextPos& pos) { focus = pos; }
	const TextPos& getFocus() const { return focus; }
	void clearSelection() { anchor = focus = TextPos{}; }
	// 全选。返回 false 表示没有可选内容（未识别 / 识别结果为空）。
	bool selectAll();
	bool hasSelection() const { return !(anchor == focus); }
	// 归一化后的 [begin, end)
	void getOrdered(TextPos& begin, TextPos& end) const;
	// 第 line 行被选中的字符区间 [b, e)。返回 false 表示该行没有选中内容。
	bool getLineRange(int line, int& b, int& e) const;
	std::wstring getSelectedText() const;

	// ---- 命中测试（原图坐标 -> 文本流位置）----
	TextPos hitTestImage(float px, float py) const;

	// 选区变化时由改动方调用；两侧控件在这里重画自己的 surface。
	// 不走 win->refresh()，避免拖拽每帧都触发一次完整 yoga 布局。
	std::function<void()> onSelectionChanged;
	void notifyChanged() const { if (onSelectionChanged) onSelectionChanged(); }

private:
	int caretInLine(int line, float px) const;
private:
	std::vector<OcrLine> lines;
	TextPos anchor{}, focus{};
};
