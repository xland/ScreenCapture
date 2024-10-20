#pragma once

#include <QWidget>

class CutMask  : public QWidget
{
	Q_OBJECT

public:
	CutMask(QWidget *parent = nullptr);
	~CutMask();
public:
	QRect maskRect;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const QPoint& pos);
private:
	qreal maskStroke{ 1.5 };
	bool dragging = false;
	QPoint dragPosition;
	int mousePosState{ -1 };
};
