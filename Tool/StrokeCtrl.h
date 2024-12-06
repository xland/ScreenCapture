#pragma once

#include <QObject>
#include <QSlider>

class StrokeCtrl:public QSlider
{
	Q_OBJECT

public:
	StrokeCtrl(QWidget *parent = nullptr);
	~StrokeCtrl();
protected:
	void paintEvent(QPaintEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
private:
	void setPosByMouse(const QPoint& pos);
	qreal getSliderXPos();
private:
	void setTip();
};
