#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "WinBase.h"

class WinPinBtns;
class WinPin  : public WinBase
{
	Q_OBJECT

public:
	WinPin(const QPoint& pos, QImage& img,QWidget *parent = nullptr);
	~WinPin();
	void resetTool();
	qreal scaleNum{ 1.0 };
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void moveEvent(QMoveEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void enterEvent(QEnterEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	QImage getTargetImg() override;
private:
	void initWindow();
private:
	QPointF posPress;
	WinPinBtns* btns;
	QSize initSize;
};
