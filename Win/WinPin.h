#pragma once

#include <QWidget>
#include "WinBase.h"

class WinPin  : public WinBase
{
	Q_OBJECT

public:
	WinPin(QWidget *parent = nullptr);
	~WinPin();
	void showToolMain() override;
	void showToolSub() override;
	void closeWin() override;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
};
