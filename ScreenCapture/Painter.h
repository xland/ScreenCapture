#pragma once
#include "blend2d.h"
#include <Windows.h>
class Painter
{
public:
	Painter();
	~Painter();
	void PaintOnWindow(HWND hwnd);
	float x, y, w, h, scaleFactor;
	bool isDrawing = false;
	BLImage* canvasImage;
	BLImage* bgImage;
	BLImage* boardImage;
	BLImage* prepareImage;
	BLContext* paintCtx;
	HBITMAP bgHbitmap;
private:
	void shotScreen();
	void initCanvas(char* bgPixels, char* boardPixels);
	void paintBoard();
};

