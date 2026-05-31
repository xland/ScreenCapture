#include "pch.h"
#include "../Win/WinPin.h"
#include "../Win/WinToolMain.h"
#include "../Win/WinToolSub.h"
#include "../Util.h"
#include "ShapeNumber.h"

static int numVal{ 0 };

ShapeNumber::ShapeNumber(WinPin* win) :ShapeBase(win), draggers{
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0),
	D2D1::RectF(0,0,0,0) }, 
	r{ 16.f * win->dpi },
	val{++numVal}
{
	auto toolSub = WinToolSub::get();
	win->render->CreateSolidColorBrush(toolSub->getSelectedColor(), brush.GetAddressOf());
	win->render->CreateSolidColorBrush(D2D1::ColorF(0XFFFFFF), brushText.GetAddressOf());
	isFill = toolSub->selectIndex == 0;
}

ShapeNumber::~ShapeNumber()
{

}

void ShapeNumber::paint()
{
	auto render = win->render.Get();
	if (isFill) {
		render->FillGeometry(path.Get(), brush.Get());
		render->DrawTextLayout({ cx - r,cy - r }, layoutText.Get(), brushText.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	}
	else {
		render->DrawGeometry(path.Get(), brush.Get(), win->dpi); 
		render->DrawTextLayout({ cx - r,cy - r }, layoutText.Get(), brush.Get(), D2D1_DRAW_TEXT_OPTIONS_NONE);
	}	
}

void ShapeNumber::paintDragger()
{
	for (auto& dragger:draggers)
	{
		win->render->DrawRectangle(dragger, brushDragger.Get(), win->dpi);
	}
}

void ShapeNumber::mouseDrag(const float& x, const float& y, const UINT_PTR& modifiers)
{
	(void)modifiers;
	if (hoverDraggerIndex == 0) {
		auto spanX{ x - pressX };
		auto spanY{ y - pressY };
		cx += spanX;
		cy += spanY;
		makePath();
		makeTextLayout();
		pressX = x;
		pressY = y;
	}
	else if (hoverDraggerIndex == 1) {
		angle = -atan2f(y - cy, x - cx) * 180.f / 3.14159265358979323846f;
		makePath();
	}
	else if (hoverDraggerIndex == 2) {
		auto dx{ x - cx };
		auto dy{ y - cy };
		r = sqrtf(dx * dx + dy * dy);
		auto minR{ 8.f * win->dpi };
		if (r < minR) r = minR;
		makePath();
		makeTextLayout();
	}
}

void ShapeNumber::mouseDown(const float& x, const float& y)
{
	if (hoverDraggerIndex == -1) { //首次创建
		cx = x;
		cy = y;
		pressX = cx;
		pressY = cy;
		hoverDraggerIndex = 0;
		makePath();
		makeTextLayout();
		win->refresh();
	}
	else if (hoverDraggerIndex >= 0) {
		pressX = x;
		pressY = y;
	}
}

void ShapeNumber::mouseUp(const float& x, const float& y)
{
	auto half{ draggerSize / 2 };
	draggers[0].left = cx - half;
	draggers[0].top = cy - half;
	draggers[0].right = cx + half;
	draggers[0].bottom = cy + half;

	draggers[1].left = tip.x - half;
	draggers[1].top = tip.y - half;
	draggers[1].right = tip.x + half;
	draggers[1].bottom = tip.y + half;


	draggers[2].left = mid.x - half;
	draggers[2].top = mid.y - half;
	draggers[2].right = mid.x + half;
	draggers[2].bottom = mid.y + half;
}

void ShapeNumber::mouseMove(const float& x, const float& y)
{
    hoverDraggerIndex = -1;
    if (Util::isInRect(draggers[0], x, y))
    {
        hoverDraggerIndex = 0;
    }
    else if (Util::isInRect(draggers[1], x, y))
    {
        hoverDraggerIndex = 1;
    }
    else if (Util::isInRect(draggers[2], x, y))
    {
        hoverDraggerIndex = 2;
    }
}

void ShapeNumber::setCursor()
{
	if (hoverDraggerIndex >=0) {
		win->setCursor(IDC_SIZEALL);
	}
}

D2D1_POINT_2F ShapeNumber::localPoint(const float& degrees)
{
	float radians = degrees * 3.14159265358979323846f / 180.f;
	return D2D1::Point2F(r * cosf(radians), -r * sinf(radians));	
}

D2D1_POINT_2F ShapeNumber::transformPoint(const D2D1_POINT_2F& point)
{
	float radians = -angle * 3.14159265358979323846f / 180.f;
	float cosValue = cosf(radians);
	float sinValue = sinf(radians);
	return D2D1::Point2F(
		cx + point.x * cosValue - point.y * sinValue,
		cy + point.x * sinValue + point.y * cosValue
	);
}

void ShapeNumber::makePath()
{
	path.Reset();
	win->getD2D()->CreatePathGeometry(path.GetAddressOf());
	ComPtr<ID2D1GeometrySink> sink;
	path->Open(sink.GetAddressOf());
	auto start = transformPoint(localPoint(10.f));
	mid = transformPoint(localPoint(180.f));
	auto end = transformPoint(localPoint(350.f));
	tip = transformPoint(D2D1::Point2F(r + r / 3.f, 0.f));
	sink->BeginFigure(start, D2D1_FIGURE_BEGIN_FILLED);
	sink->AddArc(D2D1::ArcSegment( mid, D2D1::SizeF(r, r), 0.f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL));
	sink->AddArc(D2D1::ArcSegment( end, D2D1::SizeF(r, r), 0.f, D2D1_SWEEP_DIRECTION_COUNTER_CLOCKWISE, D2D1_ARC_SIZE_SMALL ));
	sink->AddLine(tip);
	sink->AddLine(start);
	sink->EndFigure(D2D1_FIGURE_END_CLOSED);
	sink->Close();
}

void ShapeNumber::makeTextLayout()
{
	layoutText.Reset();
	auto valStr = std::to_wstring(val);
	auto d = r * 2;
	auto format = win->getIconFormat();
	auto dwrite = win->getWriteFactory();
	dwrite->CreateTextLayout(valStr.data(), (UINT32)valStr.length(), format, d, d, layoutText.GetAddressOf());
	layoutText->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	layoutText->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	layoutText->SetFontSize(r, { 0, static_cast<UINT32>(valStr.length()) });
}
