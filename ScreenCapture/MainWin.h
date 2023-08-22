#pragma once
#include <blend2d.h>
#include <windows.h>
#include <vector>

#include "WindowBase.h"

class MainWin :public WindowBase
{
public:
	~MainWin();
	static void Init();
	static MainWin* Get();
	static void Dispose();
private:
	MainWin();
	void enumDesktopWindow();
	std::vector<BLBox> desktopWindowBoxes;

	//mask
	void SetCutBoxWhenLeftButtonDown() override;
	void CheckMouseEnterMaskBox(const int& x, const int& y) override;
	void SetCutBox(const int& x, const int& y) override;
	void SetCutBox(const int& x1, const int& y1, const int& x2, const int& y2) override;
	void HighLightWindowBox(const int& x, const int& y) override;
	void DrawMaskBox() override;
	
	BLBox maskBoxes[8];	
	BLPoint dragStartCutBoxStartPos;
	BLPoint dragStartCutBoxEndPos;
};

