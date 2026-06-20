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
	static std::wstring getSaveFilePath(HWND hwnd, const std::wstring& ext = L"png");
	static std::wstring getTextFromClipboard();
	static std::vector<BYTE> captureScreen(const int& x, const int& y, const int& w, const int& h);
	static bool isInRect(const D2D1_RECT_F& rect, const float& x, const float& y);
	static std::tuple<int, int, int, int> getDesktopInfo();
	static std::wstring createFileName(const std::wstring& ext);
	static std::wstring convertToWStr(const char* str);
	static std::string convertToStr(const std::wstring& wstr);
	static void addFileToClipboard(const std::wstring& filePath);
	static std::wstring readFile(const std::wstring& path);
	static void saveFile(const std::wstring& path,const std::wstring& content);
	static std::vector<std::wstring> splitStr(const std::wstring& str, wchar_t delimiter);
	static std::wstring keyToStr(UINT vkCode);
	static UINT strToKey(const std::wstring& vkCode);
	static void setAutoStart(bool flag);
};

