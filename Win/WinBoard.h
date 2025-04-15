#pragma once

#include <QWidget>
#include "WinBase.h"

class WinBoard : public WinBase
{
	Q_OBJECT
public:
	WinBoard(QWidget *parent = nullptr);
	~WinBoard();
	void refresh(bool releaseFlag=true);
private:
	bool refreshFlag{ false };
};
