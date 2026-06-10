#pragma once
#include "pch.h"
class Util
{
public:
	static void moveMouse(const int& key);
	static void trackMouse(HWND hwnd, bool cancel = false);
	static void saveToClipboard(int& w, int& h, BYTE* data);
	static bool saveToFile(const std::wstring& path,const int& w,const int& h, BYTE* data);
	static void setTextToClipboard(const std::wstring& text);
	static std::wstring getSaveFilePath(HWND hwnd);
	static std::wstring getTextFromClipboard();
	static std::vector<BYTE> captureScreen(const int& x, const int& y, const int& w, const int& h);
	static bool isInRect(const D2D1_RECT_F& rect, const float& x, const float& y);
};

