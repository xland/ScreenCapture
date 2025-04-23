#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>


#include "ToolBase.h"


class BtnBase;
class BtnCheck;
class ToolMain : public ToolBase
{
	Q_OBJECT
public:
	ToolMain(QWidget* parent = nullptr);
	~ToolMain();
	void setBtnEnable(const QString& name, bool flag);
	void confirmPos();
	void btnCheckChange(BtnCheck* btn) override;
	void btnClick(Btn* btn) override;
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	bool topFlag{false};
	std::vector<BtnBase> btns;
};
