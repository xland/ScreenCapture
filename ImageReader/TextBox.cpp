#include "pch.h"
#include "TextBox.h"
#include <Windows.UI.Composition.Interop.h>
#include <algorithm>

using namespace Microsoft::WRL;

namespace {
	// 内边距（逻辑像素）。padding 只在这里定义一次，
	// loadText / relayoutLines / 命中测试都从这里取，避免各处硬编码 12 走偏。
	constexpr float kPad{ 12.f };
	// 自动滚动定时器 id（WinBase::setTimer 内部会加 WM_APP）
	constexpr UINT kAutoScrollTimer{ 41 };
}

TextBox::TextBox(Ling::WinBase* win) : Ling::Node(win)
{
	setWidthPercent(100.f);
	setPadding(kPad);
	win->onMouseDown.add([this](POINT pt, bool isRight) {this->onDown(pt, isRight);});
	win->onMouseMove.add([this](POINT pt) {this->onMove(pt);});
	win->onMouseUp.add([this](POINT pt, bool isRight) {this->onUp(pt);});
	win->onTimer.add([this](UINT id) {this->onTimer(id);});
}

TextBox::~TextBox()
{

}

Ling::ScrollerBox* TextBox::getScroller() const
{
	// ScrollerBox 把子节点挂在它内部的 content 节点下，所以父级要往上找两层
	for (auto p = parent; p; p = p->parent) {
		if (auto sb = dynamic_cast<Ling::ScrollerBox*>(p)) return sb;
	}
	return nullptr;
}

void TextBox::loadText(OcrDoc* doc)
{
	this->doc = doc;
	lineLayouts.clear();
	linePoss.clear();
	if (!doc) return;
	auto d2d = Ling::D2D::get();
	const float textW = (std::max)(1.f, w - kPad * win->dpi * 2.f);
	// lineLayouts 必须与 doc->getLines() 严格一一对应 —— 下标就是 TextPos::line。
	// 所以即使 CreateTextLayout 失败也要占位（存 nullptr），使用处判空即可，
	// 绝不能 continue 掉，否则后面所有行的高亮都会错位一行。
	for (const auto& line : doc->getLines()) {
		ComPtr<IDWriteTextLayout> textLayout;
		//设置最大宽度，允许自动换行
		d2d->dwriteFactory->CreateTextLayout(line.text.data(), (UINT32)line.text.length(),
			d2d->baseTextFormat.Get(), textW, FLT_MAX, textLayout.ReleaseAndGetAddressOf());
		if (textLayout) {
			textLayout->SetWordWrapping(DWRITE_WORD_WRAPPING_WRAP);
			textLayout->SetFontSize(14.f * win->dpi, { 0, INT_MAX });
		}
		lineLayouts.push_back(std::move(textLayout));
		linePoss.push_back({});
	}
	relayoutLines();
}

bool TextBox::relayoutLines()
{
	// 宽度变了 -> 换行数变了 -> 每行高度变了。必须重新累加 y，
	// 否则 linePoss 还是旧值，文字会互相重叠、高亮也跟着错位。
	const float pad = kPad * win->dpi;
	const float textW = (std::max)(1.f, w - pad * 2.f);
	float cur = pad;
	for (size_t i = 0; i < lineLayouts.size(); ++i) {
		linePoss[i] = { pad, cur };
		if (!lineLayouts[i]) continue;
		lineLayouts[i]->SetMaxWidth(textW);
		DWRITE_TEXT_METRICS m{};
		lineLayouts[i]->GetMetrics(&m);
		cur += m.height;
	}
	const float wantH = cur + pad;
	// setHeight 收逻辑像素
	const float wantLogical = wantH / win->dpi;
	if (std::abs(h - wantH) > 0.5f) {
		setHeight(wantLogical);
		return true;
	}
	return false;
}

void TextBox::paint()
{
	const int pxW = static_cast<int>(w);
	const int pxH = static_cast<int>(h);
	if (pxW <= 0 || pxH <= 0 ) return;
	if (!surface) {
		auto d2d = Ling::D2D::get();
		surface = d2d->createDrawingSurface(win->compositor, (float)pxW, (float)pxH);
		auto surfaceBrush = win->compositor.CreateSurfaceBrush(surface);
		surfaceBrush.Stretch(winrt::Windows::UI::Composition::CompositionStretch::None);
		visual.Brush(surfaceBrush);
	}
	else {
		auto sz = surface.SizeInt32();
		if (sz.Width != pxW || sz.Height != pxH) {
			surface.Resize({ pxW, pxH });
		}
	}
	auto s = surface.as<ABI::Windows::UI::Composition::ICompositionDrawingSurfaceInterop>();
	ComPtr<ID2D1DeviceContext> ctx;
	POINT offset{};
	s->BeginDraw(nullptr, __uuidof(ID2D1DeviceContext), reinterpret_cast<void**>(ctx.GetAddressOf()), &offset);
	if (!textBrush) {
		ctx->CreateSolidColorBrush(Ling::Color(0x333333FF).getD2DColor(), &textBrush);
	}
	if (!selBrush) {
		// 选中底色：比图像侧稍不透明，因为要垫在文字下面而不是盖在字上
		ctx->CreateSolidColorBrush(Ling::Color(0xb7eb8fFF).getD2DColor(), &selBrush);
	}
	ctx->SetTransform(D2D1::Matrix3x2F::Translation((float)offset.x, (float)offset.y));
	ctx->Clear(0);
	// 先铺选中底色，再画文字 —— 文字才在色块之上
	drawSelection(ctx.Get());
	auto index = 0;
	for (const auto& lineLayout : lineLayouts) {
		if (lineLayout) ctx->DrawTextLayout(linePoss[index], lineLayout.Get(), textBrush.Get());
		index++;
	}
	s->EndDraw();
}

void TextBox::drawSelection(ID2D1DeviceContext* ctx)
{
	if (!doc || !doc->hasSelection() || !selBrush) return;
	for (size_t li = 0; li < lineLayouts.size(); ++li) {
		if (!lineLayouts[li]) continue;
		int b, e;
		if (!doc->getLineRange((int)li, b, e)) continue;
		// 用 DWrite 自己算命中矩形 —— 自动换行后一个区间会拆成多个矩形，
		// 这样无论 splitter 怎么改宽度，色块都精确贴合字形。
		UINT32 actual = 0;
		auto* layout = lineLayouts[li].Get();
		layout->HitTestTextRange(b, e - b, linePoss[li].x, linePoss[li].y, nullptr, 0, &actual);
		if (actual == 0) continue;
		std::vector<DWRITE_HIT_TEST_METRICS> hits(actual);
		if (FAILED(layout->HitTestTextRange(b, e - b, linePoss[li].x, linePoss[li].y,
			hits.data(), actual, &actual))) continue;
		for (UINT32 i = 0; i < actual; ++i) {
			const auto& m = hits[i];
			if (m.width <= 0.f || m.height <= 0.f) continue;
			ctx->FillRectangle(D2D1::RectF(m.left, m.top, m.left + m.width, m.top + m.height), selBrush.Get());
		}
	}
}

void TextBox::layout()
{
	Node::layout();
	// 宽度可能刚被 splitter 改过：先按新宽度重排，高度变了就请求二次布局
	if (relayoutLines()) win->refresh();
	paint();
}

POINT TextBox::toLocal(POINT pt) const
{
	// x/y 是 yoga 算出的「未滚动」原点：Node::layout 累加了父级偏移，但
	// ScrollerBox 的滚动是施加在 content visual 的 Offset 上的，yoga 并不知道。
	// 屏幕 y = 内容 y + 控件原点 - scrollY，所以反推内容坐标要 +scrollY。
	float sy = 0.f;
	if (auto sb = getScroller()) sy = sb->getScrollY();
	return POINT{ pt.x - (LONG)x, pt.y - (LONG)y + (LONG)sy };
}

bool TextBox::isPosInContent(POINT pt) const
{
	if (!doc || lineLayouts.empty()) return false;
	auto sb = getScroller();
	// 必须落在 ScrollerBox 的可视矩形内（排除右侧滚动条竖带）—— 那里管裁剪
	if (sb && !sb->isPosInContent(pt)) return false;
	// 再检查是否落在 TextBox 自身范围内。注意要用滚动后的实际屏幕位置比较。
	const float sy = sb ? sb->getScrollY() : 0.f;
	const float top = y - sy, bottom = top + h;
	return pt.x >= x && pt.x < x + w && pt.y >= top && pt.y < bottom;
}

TextPos TextBox::hitTestLocal(POINT local) const
{
	if (lineLayouts.empty()) return TextPos{};
	// 找纵向落在哪个 layout 的范围内；落在行间/行外则吸附到最近的
	int best = -1;
	float bestDist = FLT_MAX;
	for (size_t i = 0; i < lineLayouts.size(); ++i) {
		if (!lineLayouts[i]) continue;
		DWRITE_TEXT_METRICS m{};
		lineLayouts[i]->GetMetrics(&m);
		const float top = linePoss[i].y;
		const float bottom = top + m.height;
		float d = (local.y < top) ? (top - local.y)
			: (local.y > bottom ? local.y - bottom : 0.f);
		if (d < bestDist) { bestDist = d; best = (int)i; }
		if (d == 0.f) break;
	}
	if (best < 0) return TextPos{};

	BOOL isTrailing = FALSE, isInside = FALSE;
	DWRITE_HIT_TEST_METRICS m{};
	if (FAILED(lineLayouts[best]->HitTestPoint(local.x - linePoss[best].x,
		local.y - linePoss[best].y, &isTrailing, &isInside, &m))) {
		return TextPos{ best, 0 };
	}
	int ch = (int)m.textPosition + (isTrailing ? (int)m.length : 0);
	const int len = (int)doc->getLines()[best].text.size();
	return TextPos{ best, std::clamp(ch, 0, len) };
}

void TextBox::onDown(POINT pt, bool isRight)
{
	if (isRight || !doc || doc->empty()) return;
	if (!isPosInContent(pt)) return;
	auto sb = getScroller();
	if (sb) {
		// 落在滚动条上时让 ScrollerBox 去处理拖动滑块，不抢
		if (pt.x >= sb->x + sb->w - sb->getScrollBarWidth()) return;
	}
	doc->setAnchor(hitTestLocal(toLocal(pt)));
	selecting = true;
	lastDragPt = pt;
	SetCapture(win->hwnd);
	doc->notifyChanged();
}

void TextBox::onMove(POINT pt)
{
	if (!selecting || !doc) return;
	lastDragPt = pt;
	// 边缘自动滚动：鼠标停在边缘不动时也要持续滚，所以用定时器驱动
	const bool wantScroll = autoScroll(pt);
	if (wantScroll && !autoScrolling) {
		autoScrolling = true;
		win->setTimer(30, kAutoScrollTimer);
	}
	else if (!wantScroll && autoScrolling) {
		autoScrolling = false;
		win->killTimer(kAutoScrollTimer);
	}
	auto pos = hitTestLocal(toLocal(pt));
	if (pos == doc->getFocus()) return;
	doc->setFocus(pos);
	doc->notifyChanged();
}

void TextBox::onTimer(UINT id)
{
	if (id != kAutoScrollTimer) return;
	if (!selecting || !doc) return;
	if (!autoScroll(lastDragPt)) return;
	// 滚动后同一个鼠标位置对应的字符变了，要跟着更新 focus
	auto pos = hitTestLocal(toLocal(lastDragPt));
	if (pos == doc->getFocus()) {
		paint();   // 至少让滚动本身反映出来
		return;
	}
	doc->setFocus(pos);
	doc->notifyChanged();
}

void TextBox::scrollFocusIntoView()
{
	auto sb = getScroller();
	if (!sb || !doc || !doc->hasSelection()) return;
	const int li = doc->getFocus().line;
	if (li < 0 || li >= (int)lineLayouts.size() || !lineLayouts[li]) return;
	DWRITE_TEXT_METRICS m{};
	lineLayouts[li]->GetMetrics(&m);
	// linePoss 是 TextBox 内容坐标；TextBox 在 ScrollerBox content 里的偏移是 y - sb->y
	const float base = y - sb->y;
	sb->scrollIntoView(base + linePoss[li].y, base + linePoss[li].y + m.height);
}

bool TextBox::autoScroll(POINT pt)
{
	auto sb = getScroller();
	if (!sb || sb->getMaxScrollY() <= 0.f) return false;
	// 拖到可视区上/下边缘外时按越界距离滚动，越远越快
	const float margin = 16.f * win->dpi;
	float delta = 0.f;
	if (pt.y < sb->y + margin) delta = pt.y - (sb->y + margin);
	else if (pt.y > sb->y + sb->h - margin) delta = pt.y - (sb->y + sb->h - margin);
	if (delta == 0.f) return false;
	const float before = sb->getScrollY();
	sb->scrollBy(std::clamp(delta, -40.f * win->dpi, 40.f * win->dpi));
	return sb->getScrollY() != before;
}

void TextBox::onUp(POINT pt)
{
	if (!selecting) return;
	selecting = false;
	if (autoScrolling) {
		autoScrolling = false;
		win->killTimer(kAutoScrollTimer);
	}
	ReleaseCapture();
}
