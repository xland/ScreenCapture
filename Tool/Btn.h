#pragma once

#include "BtnBase.h"

class Btn : public BtnBase
{
	Q_OBJECT

public:
	Btn(const QString& name, const QChar& icon,bool isEnable = true, QWidget* parent = nullptr);
	~Btn();
	void setEnable(bool flag);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
private:
	bool isEnable;

};
