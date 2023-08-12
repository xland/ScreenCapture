#pragma once
#include "blend2d.h"
#include <Windows.h>
#include <string>
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
	std::wstring GetPixelColorRgb();
	std::wstring GetPixelColorHex();
	BLContext* PaintBoard();
	void DrawPixelInfo();
	LONG pixelX{ -999999 } , pixelY{ -999999 };
	bool IsMosaicUsePen = false;
	int x, y, w, h;
	bool isDrawing = false;
	unsigned char* bgPixels;  //todo 马赛克那边可以少个变量
	BLImage* canvasImage;
	BLImage* bgImage;
	BLImage* boardImage;
	BLImage* prepareImage;
	BLImage* mosaicImage;
	BLContext* paintCtx;
private:
	Painter();
	void shotScreen();
	HBITMAP bgHbitmap;
};

