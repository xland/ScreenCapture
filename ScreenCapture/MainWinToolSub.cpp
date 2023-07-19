#include "MainWin.h"


void MainWin::drawSubToolBackground(const int& btnCount)
{
	paintCtx->setFillStyle(BLRgba32(255, 255, 255, 255));
	auto x = toolBoxMain.x0 + (double)selectedToolIndex * toolBtnWidth + toolBtnWidth / 2;
	auto y = toolBoxMain.y1 + toolBoxSpan / 2;
	paintCtx->fillTriangle(x, y, x + 6, y + toolBoxSpan / 2, x - 6, y + toolBoxSpan / 2);

	toolBoxSub.x0 = toolBoxMain.x0;
	toolBoxSub.y0 = toolBoxMain.y1 + toolBoxSpan;
	toolBoxSub.x1 = toolBoxSub.x0 + btnCount*toolBtnWidth;
	toolBoxSub.y1 = toolBoxSub.y0 + toolBoxHeight;
	paintCtx->fillBox(toolBoxSub);
}

void MainWin::drawSubTool()
{
	switch (selectedToolIndex)
	{
		case 0:
		{
			drawSubToolRect();
			break;
		}
		case 1:
		{
			drawSubToolEllipse();
			break;
		}
		case 2:
		{
			drawSubToolArrow();
			break;
		}
		case 3:
		{
			drawSubToolNumber();			
			break;
		}
		case 4:
		{
			drawSubToolLine();		
			break;
		}
		case 5:
		{
			drawSubToolPen();
			break;
		}
	}
}
void MainWin::drawSubToolRect()
{
	drawSubToolBackground(isFill?9:12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtn(point, Icon::Name::rectFill, isFill, mouseEnterSubToolIndex == 0);	
	if (!isFill) {
		point.x += toolBtnWidth;
		drawStrokeWidthBtns(point, 1);
	}
	drawColorBtns(point,4);
}
void MainWin::drawSubToolEllipse()
{
	drawSubToolBackground(isFill ? 9 : 12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtn(point, Icon::Name::ellipseFill, isFill, mouseEnterSubToolIndex == 0);	
	if (!isFill) {
		point.x += toolBtnWidth;
		drawStrokeWidthBtns(point, 1);
	}	
	drawColorBtns(point,4);
}
void MainWin::drawSubToolArrow()
{
	drawSubToolBackground(isFill ? 9 : 12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtn(point, Icon::Name::arrowFill, isFill, mouseEnterSubToolIndex == 0);	
	if (!isFill) {
		point.x += toolBtnWidth;
		drawStrokeWidthBtns(point, 1);
	}
	drawColorBtns(point,4);
}
void MainWin::drawSubToolNumber()
{
	drawSubToolBackground(isFill ? 9 : 12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtn(point, Icon::Name::numberFill, isFill, mouseEnterSubToolIndex == 0);
	if (!isFill) {
		point.x += toolBtnWidth;
		drawStrokeWidthBtns(point, 1);
	}
	drawColorBtns(point, 4);
}

void MainWin::drawSubToolPen()
{
	drawSubToolBackground(11);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawStrokeWidthBtns(point,0);
	drawColorBtns(point,3);
}

void MainWin::drawSubToolLine()
{
	drawSubToolBackground(12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtn(point, Icon::Name::transparent, !isFill, mouseEnterSubToolIndex == 0);
	point.x += toolBtnWidth;
	drawStrokeWidthBtns(point, 1);
	drawColorBtns(point, 4);
}


void MainWin::drawColorBtns(BLPoint& point,const int& index)
{
	BLRgba32 colors[8] = {
		BLRgba32(207, 19, 34, 255),
		BLRgba32(212, 136, 6, 255),
		BLRgba32(56, 158, 13, 255),
		BLRgba32(19, 194, 194, 255),
		BLRgba32(9, 88, 217, 255),
		BLRgba32(114, 46, 209, 255),
		BLRgba32(235, 47, 150, 255),
		BLRgba32(0, 0, 0, 255),
	};
	for (size_t i = 0; i < 8; i++)
	{
		point.x += toolBtnWidth;
		paintCtx->setFillStyle(colors[i]);
		Icon::Name name = i == colorBtnIndex ? Icon::Name::colorChecked : Icon::Name::colorUnChecked;
		drawBtn(point, name, i==colorBtnIndex, mouseEnterSubToolIndex == index+i,true);
	}
}
void MainWin::drawStrokeWidthBtns(BLPoint& point,const int& index)
{
	drawBtn(point, Icon::Name::dot, strokeBtnIndex == 0, mouseEnterSubToolIndex == index);
	point.x += toolBtnWidth;
	fontIcon->setSize(56.0f);
	drawBtn(point, Icon::Name::dot, strokeBtnIndex == 1, mouseEnterSubToolIndex == index+1);
	point.x += toolBtnWidth;
	fontIcon->setSize(86.0f);
	drawBtn(point, Icon::Name::dot, strokeBtnIndex == 2, mouseEnterSubToolIndex == index+2);
	fontIcon->setSize(28.0f);
}

void MainWin::subToolBtnClick()
{
	switch (selectedToolIndex)
	{
		case 0:
		case 1:
		case 2:
		case 3:
		case 4:
		{
			clickSubToolNormal();
			break;
		}
		case 5:
		{
			clickSubToolPen();
			break;
		}
	}
}
void MainWin::clickSubToolNormal()
{
	auto index = mouseEnterSubToolIndex;
	if (index != 0 && isFill) index += 3;
	switch (index)
	{
		case 0:
		{
			isFill = !isFill;
			break;
		}
		case 1:
		case 2:
		case 3:
		{
			strokeBtnIndex = index -1;
			break;
		}
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		{
			colorBtnIndex = index - 4;
			break;
		}
	}
	InvalidateRect(hwnd, nullptr, false);
}
void MainWin::clickSubToolPen()
{
	switch (mouseEnterSubToolIndex)
	{
	case 0:
	case 1:
	case 2:
	{
		strokeBtnIndex = mouseEnterSubToolIndex;
		break;
	}
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
	case 9:
	case 10:
	{
		colorBtnIndex = mouseEnterSubToolIndex - 3;
		break;
	}
	}
	InvalidateRect(hwnd, nullptr, false);
}