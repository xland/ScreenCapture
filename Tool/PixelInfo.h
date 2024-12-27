#pragma once

#include <QWidget>

class WinBox;
class PixelInfo  : public QWidget
{
	Q_OBJECT

public:
	PixelInfo(WinBox* win, QWidget* parent=nullptr);
	~PixelInfo();
	void mouseMove(const QPoint& nativePos);
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	bool posInScreen(const int& x, const int& y);
private:
	WinBox* win;
	QPoint nativePos;
};
