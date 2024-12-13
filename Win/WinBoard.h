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
private:
	bool refreshFlag{ false };
};
