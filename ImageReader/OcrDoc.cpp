#include "pch.h"
#include <TinyOCR/include/tinyocr/tiny_ocr.h>
#include <algorithm>
#include <cfloat>
#include "OcrDoc.h"

using namespace Microsoft::WRL;

void OcrDoc::clear()
{
	lines.clear();
	anchor = focus = TextPos{};
}

void OcrDoc::loadResult(const tinyocr::Result& result)
{
	clear();
	auto d2d = Ling::D2D::get();
	lines.reserve(result.lines.size());

	for (const auto& L : result.lines) {
		OcrLine out;
		out.text = Ling::Util::convertToWStr(L.text.c_str());
		out.charQuads.assign(out.text.size(), Quad{});
		out.charGeoms.assign(out.text.size(), nullptr);

		// word box 的文本拼接不一定等于行文本：CTC 分词丢掉了空白字符
		// （ctc_decoder.cpp 的 string_is_space 分支），cal_word_boxes 末尾还会按
		// min(contents, cells, confs) 截断。所以只能顺序扫描着对齐，
		// 不能假设 words[i] 就是 text[i]。
		size_t cursor = 0;
		for (const auto& W : L.words) {
			auto wt = Ling::Util::convertToWStr(W.text.c_str());
			if (wt.empty()) continue;
			size_t at = out.text.find(wt, cursor);
			if (at == std::wstring::npos) continue;   // 对不上就放弃这个框，不错位
			Quad q;
			for (int k = 0; k < 4; ++k) q.p[k] = { W.box[k].x, W.box[k].y };
			q.valid = q.right() > q.left() && q.bottom() > q.top();
			if (q.valid) {
				// 一个 word 覆盖多个字符时（本项目 return_single_char_box=true，
				// 基本只有 1 个）按 x 均分，保证逐字符命中仍然准。
				const size_t n = wt.size();
				const float l = q.left(), r = q.right(), t = q.top(), b = q.bottom();
				for (size_t k = 0; k < n; ++k) {
					Quad cq;
					if (n == 1) cq = q;
					else {
						float x0 = l + (r - l) * k / n;
						float x1 = l + (r - l) * (k + 1) / n;
						cq.p[0] = { x0, t }; cq.p[1] = { x1, t };
						cq.p[2] = { x1, b }; cq.p[3] = { x0, b };
					}
					cq.valid = true;
					out.charQuads[at + k] = cq;
				}
			}
			cursor = at + wt.size();
		}

		// 预建每字符的路径几何：拖拽期间只组装不重建
		bool first = true;
		for (size_t i = 0; i < out.charQuads.size(); ++i) {
			const auto& q = out.charQuads[i];
			if (!q.valid) continue;
			ComPtr<ID2D1PathGeometry> path;
			if (FAILED(d2d->d2dFactory->CreatePathGeometry(path.GetAddressOf()))) continue;
			ComPtr<ID2D1GeometrySink> sink;
			if (FAILED(path->Open(sink.GetAddressOf()))) continue;
			sink->BeginFigure(q.p[0], D2D1_FIGURE_BEGIN_FILLED);
			sink->AddLine(q.p[1]);
			sink->AddLine(q.p[2]);
			sink->AddLine(q.p[3]);
			sink->EndFigure(D2D1_FIGURE_END_CLOSED);
			if (FAILED(sink->Close())) continue;
			out.charGeoms[i] = path;

			if (first) {
				out.boxLeft = q.left(); out.boxTop = q.top();
				out.boxRight = q.right(); out.boxBottom = q.bottom();
				first = false;
			}
			else {
				out.boxLeft = (std::min)(out.boxLeft, q.left());
				out.boxTop = (std::min)(out.boxTop, q.top());
				out.boxRight = (std::max)(out.boxRight, q.right());
				out.boxBottom = (std::max)(out.boxBottom, q.bottom());
			}
			out.hasGeom = true;
		}
		lines.push_back(std::move(out));
	}
}

bool OcrDoc::selectAll()
{
	// 找第一个/最后一个非空行。空行（OCR 识别出空文本）夹在两端时如果算进选区，
	// hasSelection 会为 true 但两侧都画不出任何高亮，看起来像没反应。
	int first = -1, last = -1;
	for (int i = 0; i < (int)lines.size(); ++i) {
		if (lines[i].text.empty()) continue;
		if (first < 0) first = i;
		last = i;
	}
	if (first < 0) return false;
	anchor = TextPos{ first, 0 };
	focus = TextPos{ last, (int)lines[last].text.size() };
	return true;
}

void OcrDoc::getOrdered(TextPos& begin, TextPos& end) const
{
	if (focus < anchor) { begin = focus; end = anchor; }
	else { begin = anchor; end = focus; }
}

bool OcrDoc::getLineRange(int line, int& b, int& e) const
{
	if (line < 0 || line >= (int)lines.size()) return false;
	if (!hasSelection()) return false;
	TextPos begin, end;
	getOrdered(begin, end);
	if (line < begin.line || line > end.line) return false;
	const int len = (int)lines[line].text.size();
	b = (line == begin.line) ? begin.ch : 0;
	e = (line == end.line) ? end.ch : len;
	b = std::clamp(b, 0, len);
	e = std::clamp(e, 0, len);
	return e > b;
}

std::wstring OcrDoc::getSelectedText() const
{
	if (!hasSelection()) return {};
	TextPos begin, end;
	getOrdered(begin, end);
	std::wstring out;
	for (int i = begin.line; i <= end.line && i < (int)lines.size(); ++i) {
		int b, e;
		if (!getLineRange(i, b, e)) continue;
		if (!out.empty()) out += L"\r\n";
		out.append(lines[i].text, b, e - b);
	}
	return out;
}

int OcrDoc::caretInLine(int line, float px) const
{
	const auto& L = lines[line];
	int bestIdx = -1;
	float bestDist = FLT_MAX;
	bool bestAfter = false;
	for (size_t i = 0; i < L.charQuads.size(); ++i) {
		const auto& q = L.charQuads[i];
		if (!q.valid) continue;
		const float l = q.left(), r = q.right();
		float dist;
		bool after;
		if (px < l) { dist = l - px; after = false; }
		else if (px > r) { dist = px - r; after = true; }
		else { dist = 0.f; after = px > (l + r) * 0.5f; }   // 落在字上：过半算下一个插入点
		if (dist < bestDist) { bestDist = dist; bestIdx = (int)i; bestAfter = after; }
	}
	if (bestIdx < 0) return 0;
	return bestAfter ? bestIdx + 1 : bestIdx;
}

TextPos OcrDoc::hitTestImage(float px, float py) const
{
	// 先找纵向落在哪一行的行高带内；带内可能有多行（多列排版），取 x 最近的。
	int hit = -1;
	float hitXDist = FLT_MAX;
	int nearest = -1;
	float nearestYDist = FLT_MAX;
	bool anyGeom = false;
	float minTop = FLT_MAX, maxBottom = -FLT_MAX;

	for (size_t i = 0; i < lines.size(); ++i) {
		const auto& L = lines[i];
		if (!L.hasGeom) continue;
		anyGeom = true;
		minTop = (std::min)(minTop, L.boxTop);
		maxBottom = (std::max)(maxBottom, L.boxBottom);

		if (py >= L.boxTop && py <= L.boxBottom) {
			float xd = 0.f;
			if (px < L.boxLeft) xd = L.boxLeft - px;
			else if (px > L.boxRight) xd = px - L.boxRight;
			if (xd < hitXDist) { hitXDist = xd; hit = (int)i; }
		}
		float yd = (py < L.boxTop) ? (L.boxTop - py)
			: (py > L.boxBottom ? py - L.boxBottom : 0.f);
		if (yd < nearestYDist) { nearestYDist = yd; nearest = (int)i; }
	}

	if (!anyGeom) return TextPos{};
	if (hit >= 0) return TextPos{ hit, caretInLine(hit, px) };
	// 拖到所有文字之上/之下 —— 与浏览器/PDF 一致地吸附到文档首/尾
	if (py < minTop) {
		for (size_t i = 0; i < lines.size(); ++i)
			if (lines[i].hasGeom) return TextPos{ (int)i, 0 };
	}
	if (py > maxBottom) {
		for (int i = (int)lines.size() - 1; i >= 0; --i)
			if (lines[i].hasGeom) return TextPos{ i, (int)lines[i].text.size() };
	}
	if (nearest >= 0) return TextPos{ nearest, caretInLine(nearest, px) };
	return TextPos{};
}
