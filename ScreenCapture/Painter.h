#pragma once
#include "blend2d.h"
#include <Windows.h>
class Painter
{
public:
	Painter(const Painter&) = delete;
	Painter& operator= (const Painter&) = delete;
	~Painter();
	static void Init();
	static Painter* Get();
	static void Dispose();
	void PaintOnWindow(HWND hwnd);
	float x, y, w, h;
	bool isDrawing = false;
	BLImage* canvasImage;
	BLImage* bgImage;
	BLImage* boardImage;
	BLImage* prepareImage;
	BLContext* paintCtx;
	HBITMAP bgHbitmap;
private:
	Painter();
	void shotScreen();
};

