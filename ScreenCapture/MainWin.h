#pragma once
#include "State.h"
#include <windows.h>
#include <windowsx.h>
#include <algorithm>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <wchar.h>
#include <math.h>
#include <iostream>
#include <vector>
#include "Icon.h"
#include <blend2d.h>
#include "Util.h"
#include "Shape/History.h"
#include "Shape/Box.h"
#include "Shape/Ellipse.h"
#include "Shape/Arrow.h"
#include "Shape/Number.h"
#include "Shape/Line.h"
#include "Shape/Pen.h"
#include "Shape/Image.h"
#include "Shape/Text.h"
#include "Font.h"

class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	static LRESULT CALLBACK RouteWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

private:




	void shotScreen();
	void initScaleFactor();
	void createWindow();
	void showWindow();
	void setCursor(LPCTSTR cursor);
	float x, y, w, h,scaleFactor;
	HWND hwnd;
	HINSTANCE hinstance;


	
	void leftBtnDown(const POINT& pos);
	void rightBtnDown(const POINT& pos);
	void mouseMove(const POINT& pos);
	void leftBtnUp(const POINT& pos);
	inline POINT getMousePoint(const LPARAM& lParam);
	POINT mouseDownPos;
	bool isLeftBtnDown;
	
	
	void initCanvas(char* bgPixels,char* boardPixels);
	void paintBoard();
	void drawShape(const POINT& pos);
	void drawPen(const POINT& pos);
	void drawLine(const POINT& pos);
	void drawEraser(const POINT& pos);
	bool endDrawing();
	BLImage* canvasImage;
	BLImage* bgImage; 
	BLImage* boardImage;
	BLImage* prepareImage;
	BLContext* paintCtx;
	HBITMAP bgHbitmap;
	BLRgba32 colors[8] {
		BLRgba32(207, 19, 34, 255),
		BLRgba32(212, 136, 6, 255),
		BLRgba32(56, 158, 13, 255),
		BLRgba32(19, 194, 194, 255),
		BLRgba32(9, 88, 217, 255),
		BLRgba32(114, 46, 209, 255),
		BLRgba32(235, 47, 150, 255),
		BLRgba32(0, 0, 0, 255),
	};
	double strokeWidths[3]{ 2.0, 6.0, 16.0 };
	bool isDrawing = false;
	State state = State::start;
	State preState;


	void checkMouseEnterMaskBox(const POINT& pos);
	void setCutBox(const POINT& pos);
	void setCutBox(BLPoint& startPos, BLPoint& endPos);
	BLBox maskBoxes[8];
	BLBox cutBox;
	BLPoint dragStartCutBoxStartPos;
	BLPoint dragStartCutBoxEndPos;
	int mouseInMaskBoxIndex = -1;




	void drawSplitter(const BLPoint& point);
	void drawBtnBackground(const BLPoint& point, const BLRgba32& color);
	void drawBtn(const BLPoint& point, Icon::Name name,bool isChecked,bool hover,bool customColor = false);
	bool checkMouseEnterToolBox(const POINT& pos);
	int mainToolBtnCount = 14;
	int toolBoxSpan = 12;//工具条距离截图区域的高度
	int toolBtnSpanWidth = 6;
	int toolBtnSpanCount = 2;
	int toolBtnWidth = 60;
	int toolBoxWidth = mainToolBtnCount * toolBtnWidth + toolBtnSpanWidth * toolBtnSpanCount;
	int toolBoxHeight = 56;
	int iconLeftMargin = 16;
	int iconTopMargin = 38;


	void setToolBoxMain();
	void drawToolMain();
	BLBox toolBoxMain;
	int mouseEnterMainToolIndex = -1;
	int selectedToolIndex = -1;
	bool canRedo = false;
	bool canUndo = false;


	void drawSubTool();
	void drawSubToolBackground(const int& btnCount);	
	void drawSubToolNormal(const Icon::Name& icon);
	void drawSubToolPen();
	void drawSubToolLine();
	void drawSubToolNumber();
	void drawColorBtns(BLPoint& point, const int& index);
	void drawStrokeWidthBtns(BLPoint& point, const int& index);
	void subToolBtnClick();
	void clickSubToolNormal();
	void clickSubToolPen();
	BLBox toolBoxSub;
	int mouseEnterSubToolIndex = -1;
	bool isFill = false;
	int strokeBtnIndex = 1;
	int colorBtnIndex = 0;


	void showDragger();

};

