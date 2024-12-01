#pragma once

#include <QWidget>
#include "WinBaseLayer.h"

class WinBoard : public WinBaseLayer
{
	Q_OBJECT
public:
	WinBoard(QWidget *parent = nullptr);
	~WinBoard();
	void refresh();
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	bool refreshFlag{ false };
};
