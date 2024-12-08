#pragma once

#include <QWidget>
#include "WinBase.h"

class WinBoard : public WinBase
{
	Q_OBJECT
public:
	WinBoard(QObject *parent = nullptr);
	~WinBoard();
	void refresh();
protected:
	//void paintEvent(QPaintEvent* event) override;
private:
	bool refreshFlag{ false };
};
