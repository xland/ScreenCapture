#pragma once
#include "pch.h"
class Util
{
public:
	static void createShadow(HWND hwnd);
	static std::wstring getContentType(const std::wstring& fileName);
	static std::wstring getResName(const std::wstring& url);
	static winrt::com_ptr<IStream> getResStream(const std::wstring& resName);
	static ID2D1Factory* getD2D();
	static IDWriteFactory5* getWriteFactory();
	static winrt::com_ptr<ID2D1HwndRenderTarget> createRender(HWND& hwnd, int& w, int& h);
	static void enableHwndAlpha(HWND hwnd);
	static IDWriteTextFormat* getTextFormat(const float& fontSize);
	static IDWriteTextFormat* getIconFormat();
	static winrt::com_ptr<IDWriteTextLayout> getIconLayout(const std::wstring& icon, const float& fontSize, const float& w, const float& h);
	static winrt::com_ptr<IDWriteTextLayout> getTextLayout(const std::wstring& str, const float& fontSize, const float& w, const float& h);
	static void moveMouse(const int& key);
	static void trackMouse(HWND hwnd, bool cancel = false);
	static void saveToClipboard(int& w, int& h, BYTE* data);
	static std::wstring getSaveFilePath(HWND hwnd);
	static winrt::com_ptr<ID2D1PathGeometry> createArrow(const D2D1_POINT_2F& startPos, const D2D1_POINT_2F& endPos);
	static void setTextToClipboard(const std::wstring& text);
	static std::wstring getTextFromClipboard();
};

