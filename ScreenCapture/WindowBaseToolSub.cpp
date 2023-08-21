#include "WindowBase.h"


void WindowBase::drawSubToolBackground(const int& btnCount, const bool isCenter)
{
	PaintCtx->setFillStyle(BLRgba32(255, 255, 255, 255));
	auto x = toolBoxMain.x0 + (double)SelectedToolIndex * toolBtnWidth + toolBtnWidth / 2 ;
	auto y = toolBoxMain.y1 + toolBoxSpan / 2;
	PaintCtx->fillTriangle(x, y, x + 6, y + toolBoxSpan / 2, x - 6, y + toolBoxSpan / 2);
	if (isCenter) {
		toolBoxSub.x0 = x- btnCount * toolBtnWidth/2;
		toolBoxSub.y0 = toolBoxMain.y1 + toolBoxSpan;
		toolBoxSub.x1 = x + btnCount * toolBtnWidth / 2;
		toolBoxSub.y1 = toolBoxSub.y0 + toolBoxHeight;
	}
	else
	{
		toolBoxSub.x0 = toolBoxMain.x0;
		toolBoxSub.y0 = toolBoxMain.y1 + toolBoxSpan;
		toolBoxSub.x1 = toolBoxSub.x0 + btnCount * toolBtnWidth;
		toolBoxSub.y1 = toolBoxSub.y0 + toolBoxHeight;
	}
	PaintCtx->fillBox(toolBoxSub);
}

void WindowBase::drawSubTool()
{
	switch (SelectedToolIndex)
	{
		case 0:
		{
			drawSubToolNormal(Icon::Name::rectFill);
			break;
		}
		case 1:
		{
			drawSubToolNormal(Icon::Name::ellipseFill);
			break;
		}
		case 2:
		{
			drawSubToolNormal(Icon::Name::arrowFill);
			break;
		}
		case 3://Number		
		{
			drawSubToolNormal(Icon::Name::numberFill);
			break;
		}
		case 4:
		{
			drawSubToolPen();
			break;
		}
		case 5://Line
		{
			drawSubToolLine();
			break;			
		}
		case 6: //Text
		{
			drawSubToolText();
			break;
		}
		case 7: //Mosaic
		{
			drawSubToolMosaic();
			break;
		}
		case 8: //Eraser
		{
			drawSubToolEraser();
			break;
		}
	}
}
void WindowBase::drawSubToolNormal(const Icon::Name& icon)
{
	drawSubToolBackground(IsFill?9:12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtnCheckable(point, icon, IsFill, mouseEnterSubToolIndex == 0);	
	if (!IsFill) {
		point.x += toolBtnWidth;
		drawStrokeWidthBtns(point, 1);
		drawColorBtns(point, 4);
	}
	else
	{
		drawColorBtns(point, 1);
	}	
}

void WindowBase::drawSubToolLine()
{
	drawSubToolBackground(12);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtnCheckable(point, Icon::Name::transparent, !IsFill, mouseEnterSubToolIndex == 0);
	point.x += toolBtnWidth;
	drawStrokeWidthBtns(point, 1);
	drawColorBtns(point, 4);
}

void WindowBase::drawSubToolPen()
{
	drawSubToolBackground(11);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawStrokeWidthBtns(point,0);
	drawColorBtns(point,3);
}

void WindowBase::drawSubToolText()
{
	drawSubToolBackground(8,true);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin - toolBtnWidth;
	point.y = toolBoxSub.y0 + 38;
	drawColorBtns(point, 0);
}

void WindowBase::drawSubToolMosaic()
{
	drawSubToolBackground(4,true);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtnCheckable(point, Icon::Name::rectFill, IsFill, mouseEnterSubToolIndex == 0);
	point.x += toolBtnWidth;
	drawStrokeWidthBtns(point, 1);
}

void WindowBase::drawSubToolEraser()
{
	drawSubToolBackground(IsFill?1:4,true);
	BLPoint point;
	point.x = toolBoxSub.x0 + iconLeftMargin;
	point.y = toolBoxSub.y0 + 38;
	drawBtnCheckable(point, Icon::Name::rectFill, IsFill, mouseEnterSubToolIndex == 0);	
	if (!IsFill) {
		point.x += toolBtnWidth;
		drawStrokeWidthBtns(point, 1);
	}
}

void WindowBase::drawColorBtns(BLPoint& point,const int& index)
{
	for (size_t i = 0; i < std::size(colors); i++)
	{
		point.x += toolBtnWidth;
		Icon::Name name = i == colorBtnIndex ? Icon::Name::colorChecked : Icon::Name::colorUnChecked;
		drawBtnColors(point, name, i==colorBtnIndex, mouseEnterSubToolIndex == index+i, colors[i]);
	}
}

void WindowBase::drawStrokeWidthBtns(BLPoint& point,const int& index)
{
	drawBtnStrokeWidth(point, strokeBtnIndex == 0, mouseEnterSubToolIndex == index);
	point.x += toolBtnWidth;
	auto font = Font::Get()->fontIcon;
	font->setSize(56.0f);
	drawBtnStrokeWidth(point, strokeBtnIndex == 1, mouseEnterSubToolIndex == index+1);
	point.x += toolBtnWidth;
	font->setSize(86.0f);
	drawBtnStrokeWidth(point, strokeBtnIndex == 2, mouseEnterSubToolIndex == index+2);
	font->setSize(28.0f);

	//BLRoundRect rect(point.x - iconLeftMargin, point.y-19, toolBtnWidth * 3, toolBoxHeight - iconTopMargin, 8);
	//PaintCtx->setFillStyle(BLRgba32(230, 230, 230));
	//PaintCtx->fillRoundRect(rect);	
	//BLRoundRect rect2(point.x - iconLeftMargin, point.y - 19, toolBtnWidth * 2, toolBoxHeight - iconTopMargin, 8);
	//PaintCtx->setFillStyle(BLRgba32(145, 202, 255));//230, 244, 255  9, 88, 217
	//PaintCtx->fillRoundRect(rect2);
	//point.x += toolBtnWidth*2;
}

void WindowBase::subToolBtnClick()
{
	switch (SelectedToolIndex)
	{
		case 0://矩形
		case 1://椭圆
		case 2://箭头
		case 3://标号
		{
			clickSubToolNormal();
			break;
		}
		case 4: //Pen
		{
			clickSubToolPen();
			break;
		}
		case 5://Line
		{
			clickSubToolNormal();
			break;
		}
		case 6://Text
		{
			clickSubToolText();
			break;
		}
		case 7://Mosaic
		{
			clickSubToolMosaic();
			break;
		}
		case 8://Eraser
		{
			clickSubToolEraser();
			break;
		}
	}
}
void WindowBase::clickSubToolNormal()
{
	auto index = mouseEnterSubToolIndex;
	if (SelectedToolIndex < 4 && index != 0 && IsFill) index += 3;
	switch (index)
	{
		case 0:
		{
			IsFill = !IsFill;
			break;
		}
		case 1:
		case 2:
		case 3:
		{
			strokeBtnIndex = index - 1;
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
	Refresh();
}
void WindowBase::clickSubToolPen()
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
	Refresh();
}

void WindowBase::clickSubToolMosaic()
{
	switch (mouseEnterSubToolIndex)
	{
		case 0:
		{
			IsFill = !IsFill;
			break;
		}
		case 1:
		case 2:
		case 3:
		{
			strokeBtnIndex = mouseEnterSubToolIndex-1;
			break;
		}
	}
	Refresh();
}

void WindowBase::clickSubToolEraser()
{
	switch (mouseEnterSubToolIndex)
	{
		case 0:
		{
			IsFill = !IsFill;
			break;
		}
		case 1:
		case 2:
		case 3:
		{
			strokeBtnIndex = mouseEnterSubToolIndex - 1;
			break;
		}
	}
	Refresh();
}

void WindowBase::clickSubToolText()
{
	colorBtnIndex = mouseEnterSubToolIndex;
	Refresh();
}