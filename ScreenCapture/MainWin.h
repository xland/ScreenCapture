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

class MainWin
{
public:
	MainWin(HINSTANCE hinstance);
	~MainWin();
	static LRESULT CALLBACK RouteWindowMessage(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
private:
	void setBoxByPos(BLBox& box, const BLPoint& startPos, const BLPoint& endPos);




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
	State state = State::start;
	
	
	void initCanvas(char* bgPixels,char* boardPixels);
	void paintBoard();
	void drawRect(const POINT& pos);
	void drawPen(const POINT& pos);
	void drawEraser(const POINT& pos);
	BLImage* canvasImage;
	BLImage* bgImage; 
	BLImage* boardImage;
	BLImage* prepareImage;

	BLContext* paintCtx;
	HBITMAP bgHbitmap;


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
	void initFontIcon();
	BLFont* fontIcon;
	BLBox toolBoxMain;
	int mouseEnterMainToolIndex = -1;
	int selectedToolIndex = -1;
	bool canRedo = false;
	bool canUndo = false;


	void drawSubTool();
	void drawSubToolBackground(const int& btnCount);
	void drawSubToolRect();
	void drawSubToolEllipse();
	void drawSubToolPen();
	void drawSubToolArrow();
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

};

