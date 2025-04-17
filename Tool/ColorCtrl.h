#pragma once
#include <QPainter>
#include <QMouseEvent>
#include <QWidget>


class ColorCtrl  : public QWidget
{
	Q_OBJECT

public:
	ColorCtrl(int selectIndex, QWidget* parent);
	~ColorCtrl();
    QColor getColor();
	int selectedIndex{ 0 };
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
private:
	std::vector<QString> getColors();
private:
	int itemWidth{ 26 };
	int hoverIndex{ -1 };
};
