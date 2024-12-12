#pragma once

#include <QWidget>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>


#include "ToolBase.h"
#include "ToolBtn.h"

class WinBox;
class ToolMain : public ToolBase
{
	Q_OBJECT
public:
	ToolMain(WinBox* win);
	~ToolMain();
	static void initData(const QJsonArray& arr, const QString& lang);
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
private:
};
