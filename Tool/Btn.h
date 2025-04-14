#pragma once

#include "BtnBase.h"

class Btn : public BtnBase
{
	Q_OBJECT

public:
	Btn(const QString& name, const QChar& icon, QWidget *parent = nullptr);
	~Btn();
protected:
	void paintEvent(QPaintEvent* event) override;
private:

};
