#pragma once
#include "pch.h"
class Util
{
public:
	static ID2D1Factory* getD2D();
	static IDWriteFactory5* getWriteFactory();
	static IDWriteTextFormat* getTextFormat(const float& fontSize);
	static IDWriteTextFormat* getIconFormat();
	static void moveMouse(const int& key);
	static void trackMouse(HWND hwnd, bool cancel = false);
	static void saveToClipboard(int& w, int& h, BYTE* data);
	static std::wstring getSaveFilePath(HWND hwnd);
	static ComPtr<ID2D1PathGeometry> createArrow(const D2D1_POINT_2F& startPos, const D2D1_POINT_2F& endPos);
	static std::wstring getTextFromClipboard();
	static std::vector<BYTE> captureScreen(const int& x, const int& y, const int& w, const int& h);
};

