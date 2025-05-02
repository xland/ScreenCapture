#pragma once
#include <QPainter>
#include <QWidget>

class WinPinBtns : public QWidget
{
	Q_OBJECT

public:
	WinPinBtns(QWidget *parent = nullptr);
	~WinPinBtns();
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
	//void enterEvent(QEvent* event) override;
private:
	int hoverIndex{-1};
};
