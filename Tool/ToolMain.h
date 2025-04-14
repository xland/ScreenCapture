#pragma once

#include <QWidget>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>


#include "ToolBase.h"
#include "ToolBtn.h"

class WinBox;
class BtnBase;
class BtnCheck;
class ToolMain : public ToolBase
{
	Q_OBJECT
public:
	ToolMain(WinBox* win);
	~ToolMain();
	void setBtnEnable(const QString& name,bool flag=true);
	void setBtnEnable(bool undo,bool redo);
	void confirmPos();
	void btnCheckChange(BtnCheck* btn) override;
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	bool topFlag{false};
	std::vector<BtnBase> btns;
};
