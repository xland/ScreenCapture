#pragma once
#include <blend2d.h>
#include <windows.h>
#include <vector>

#include "WindowBase.h"

class MainWin :public WindowBase
{
public:
	MainWin();
	~MainWin();
private:
	void initWindowSize();
	void shotScreen();
	void PinWindow() override;
	void setToolBoxPos();
	void BeforePaint() override;
	void SaveFile(const std::string& filePath) override;

	//desktop window boxes
	void enumDesktopWindow();
	void HighLightWindowBox(const int& x, const int& y) override;
	std::vector<BLBox> desktopWindowBoxes;

	//mask
	void SetCutBoxWhenLeftButtonDown() override;
	void MouseMoveWhenMaskReady(const int& x, const int& y) override;
	void SetCutBox(const int& x, const int& y) override;
	void SetCutBox(const int& x1, const int& y1, const int& x2, const int& y2) override;
	void drawMaskBoxes();
	BLBox maskBoxes[8];	
	BLPoint dragStartCutBoxStartPos;
	BLPoint dragStartCutBoxEndPos;
};

