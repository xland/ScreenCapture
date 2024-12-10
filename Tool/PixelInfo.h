#pragma once

#include <QWidget>

class WinFull;
class PixelInfo  : public QWidget
{
	Q_OBJECT

public:
	PixelInfo(WinFull* win, QWidget* parent=nullptr);
	~PixelInfo();
	void mouseMove(QMouseEvent* event);
protected:
	void paintEvent(QPaintEvent* event) override;
	bool posInScreen(const int& x,const int& y);
private:
	WinFull* win;
	QPoint nativePos;
};
