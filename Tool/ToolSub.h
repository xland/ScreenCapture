#pragma once
#include <QWidget>
#include "ToolBase.h"

class StrokeCtrl;
class ColorCtrl;
class WinBox;
class ToolSub : public ToolBase
{
	Q_OBJECT

public:
	ToolSub(WinBox* win);
	~ToolSub();
	bool getSelectState(const QString& btnName);
    QColor getColor();
    int getStrokeWidth();
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	StrokeCtrl* strokeCtrl;
	ColorCtrl* colorCtrl;
	int triangleX{ 0 };
};
