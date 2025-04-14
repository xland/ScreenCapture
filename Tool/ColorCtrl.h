#pragma once

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
	int itemWidth{ 26 };
	int hoverIndex{ -1 };
	std::vector<QString> colorValues;
	std::vector<QString> colorTips;
};
