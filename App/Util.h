#pragma once
#include <Windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <QMouseEvent>
#include <QImage>

class Util
{
public:
	static QFont* getIconFont(const int& fontSize);
	static QFont* getTextFont(const int& fontSize);
	static QMouseEvent createMouseEvent(const QEvent::Type& type = QEvent::MouseMove, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
	static QMouseEvent createMouseEvent(const LPARAM& lParam, const QEvent::Type& type, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
	static QImage printScreen(const int& x, const int& y, const int& w, const int& h);
	static QImage printScreen();
	static void imgToClipboard(const QImage& img);
	static bool posInScreen(const int& x, const int& y);
	static QScreen* getScreen(const int& x, const int& y);
	static void setClipboardText(const std::wstring& text);
	static void copyColor(const int& key);
	static QPoint getQtPoint(int x, int y);
};

