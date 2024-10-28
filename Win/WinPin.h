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
};
