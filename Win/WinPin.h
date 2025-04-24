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
	void saveToClipboard() override;
	void saveToFile() override;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void initWindow();
	void showContextMenu(const QPoint& pos);
	void moveEvent(QMoveEvent* event) override;
private:
	QPointF posPress;
};
