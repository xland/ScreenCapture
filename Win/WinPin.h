#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "WinBase.h"

class WinPin  : public WinBase
{
	Q_OBJECT

public:
	WinPin(const QRect& r, const QImage& img,QWidget *parent = nullptr);
	~WinPin();
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void moveEvent(QMoveEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	QImage getTargetImg() override;
private:
	void initWindow();
	void showContextMenu(const QPoint& pos);
private:
	QPointF posPress;
};
