#pragma once

#include <QWidget>
#include <QPainter>
#include <QMouseEvent>
#include <QPoint>
#include <QHBoxLayout>
#include <QToolTip>

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
	int btnCheckedCenterX;
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	QWidget* getTool(const QString& toolName);
	void initDefaultTool(QHBoxLayout* layout);
private:
	bool topFlag{false};
	std::vector<BtnBase> btns;
	QList<int> splitters;
	int posState{ -1 };
};
