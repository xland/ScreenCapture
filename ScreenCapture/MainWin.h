#pragma once
#include <blend2d.h>
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

#include "WindowBase.h"
#include "State.h"
#include "Icon.h"
#include "Util.h"
#include "Font.h"

#include "Shape/History.h"
#include "Shape/Box.h"
#include "Shape/Ellipse.h"
#include "Shape/Arrow.h"
#include "Shape/Number.h"
#include "Shape/Line.h"
#include "Shape/Pen.h"
#include "Shape/Text.h"
#include "Shape/Eraser.h"
#include "Shape/Mosaic.h"




class MainWin :public WindowBase
//class MainWin
{
public:
	~MainWin();
	static void Init();
	static MainWin* Get();
	static void Dispose();
	static int GetQuitCode();
	POINT MouseDownPos;
	bool IsShiftDown = false;
	bool IsCtrlDown = false;
	bool IsLeftBtnDown = false;
	bool IsDoubleClick = false;
	State state = State::start;
	State PreState;
	int SelectedToolIndex = -1;
	bool IsFill = false;
	bool IsDrawing = false;
	bool IsMosaicUsePen = false;
	BLContext* PaintCtx;
	BLImage* PrepareImage;
	BLImage* CanvasImage;
	BLImage* DesktopImage;
	BLImage* MosaicImage;
protected:
	
private:
	MainWin();
	//system
	void saveFile();
	void saveClipboard();
	void initWindowBoxes();
	void quitApp(const int& exitCode);	
	void initLayerImg();
	bool OnPaint() override;
	bool OnTimer(const unsigned int& id) override;
	bool OnKeyDown(const unsigned int& key) override;
	bool OnKeyUp(const unsigned int& key) override;
	void drawPixelInfo();
	std::wstring getPixelColorRgb();
	std::wstring getPixelColorHex();
	std::vector<BLBox> windowBoxes;
	LONG pixelX{ -999999 }, pixelY{ -999999 };
	BLImage* bottomImage;

	//mouse
	bool OnLeftButtonDown(const int& x, const int& y) override;
	bool OnRightButtonDown(const int& x, const int& y) override;
	bool OnMouseMove(const int& x, const int& y) override;
	bool OnLeftButtonUp(const int& x, const int& y) override;
	void leftBtnDownStartDraw();
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

	//mask
	void checkMouseEnterMaskBox(const POINT& pos);
	void setCutBox(const POINT& pos);
	void setCutBox(BLPoint& startPos, BLPoint& endPos);
	BLBox maskBoxes[8];
	BLBox cutBox;
	int cutBoxBorderWidth = 4;
	BLPoint dragStartCutBoxStartPos;
	BLPoint dragStartCutBoxEndPos;
	int mouseInMaskBoxIndex = -1;



	//tool
	void drawSplitter(const BLPoint& point);
	BLRoundRect getBtnRect(const BLPoint& point);
	void drawBtn(const BLPoint& point, const Icon::Name& name,const bool& hover);
	void drawBtnCheckable(const BLPoint& point, const Icon::Name& name, const bool& checked, const bool& hover);
	void drawBtnColors(const BLPoint& point, const Icon::Name& name, const bool& checked, const bool& hover,const BLRgba32& color);
	void drawBtnStrokeWidth(const BLPoint& point, const bool& checked, const bool& hover);
	void drawBtnUndoRedo(const BLPoint& point, const Icon::Name& name, const bool& hover, const bool& enable);
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

	//toolmain
	void setToolBoxMain();
	void drawToolMain();
	BLBox toolBoxMain;
	int mouseEnterMainToolIndex = -1;

	//toolsub
	void drawSubTool();
	void drawSubToolBackground(const int& btnCount,const bool isCenter=false);	
	void drawSubToolNormal(const Icon::Name& icon);
	void drawSubToolPen();
	void drawSubToolEraser();
	void drawSubToolText();
	void drawSubToolMosaic();
	void drawSubToolLine();
	void drawColorBtns(BLPoint& point, const int& index);
	void drawStrokeWidthBtns(BLPoint& point, const int& index);
	void subToolBtnClick();
	void clickSubToolNormal();
	void clickSubToolPen();
	void clickSubToolMosaic();
	void clickSubToolEraser();
	void clickSubToolText();
	BLBox toolBoxSub;
	int mouseEnterSubToolIndex = -1;
	int strokeBtnIndex = 1;
	int colorBtnIndex = 0;

};

