#pragma once
#include <QMouseEvent>
class Util
{
public:
	//static void setCursor(QWidget* widget, Qt::CursorShape cursor);
	static QMouseEvent createMouseEvent(const int& x,const int& y, const QEvent::Type& type, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
	static QMouseEvent createMouseEvent(const QEvent::Type& type = QEvent::MouseMove, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
	static QMouseEvent createMouseEvent(const LPARAM& lParam, const QEvent::Type& type, const Qt::MouseButton& btn = Qt::MouseButton::LeftButton);
};

