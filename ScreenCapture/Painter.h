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
	float x, y, w, h;
	bool isDrawing = false;
	unsigned char* bgPixels;  //todo 马赛克那边可以少个变量
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

