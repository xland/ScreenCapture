#pragma once

#include <QWidget>
#include <QMouseEvent>

class CutMask  : public QWidget
{
	Q_OBJECT

public:
	CutMask(QWidget *parent = nullptr);
	~CutMask();
    void PressEvent(QMouseEvent* event);
    void MoveEvent(QMouseEvent* event);
    void ReleaseEvent(QMouseEvent* event);
public:
	QRect maskRect;
protected:
    void paintEvent(QPaintEvent* event) override;
private:
	void changeMaskRect(const QPoint& pos);
	void changeMousePosState(const QPoint& pos);
private:
	qreal maskStroke{ 1.5 };
	QPoint dragPosition;
	int mousePosState{ -1 };
};
