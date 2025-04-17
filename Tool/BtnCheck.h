#pragma once
#include "BtnBase.h"
#include "../App/State.h"

class BtnCheck : public BtnBase
{
	Q_OBJECT

public:
	BtnCheck(const QString& name, const QChar& icon, const State& state, QWidget *parent = nullptr);
	~BtnCheck();
	State state;
	bool isChecked{ false };
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
private:
};
