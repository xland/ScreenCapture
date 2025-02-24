#pragma once

#include <QWidget>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>


#include "ToolBase.h"
#include "ToolBtn.h"

class Box;
class ToolMain : public ToolBase
{
	Q_OBJECT
public:
	ToolMain(Box* box);
	~ToolMain();
	static void initData(const QJsonArray& arr, const QString& lang);
	void setBtnEnable(const QString& name,bool flag=true);
	void setBtnEnable(bool undo,bool redo);
	void confirmPos();
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	bool topFlag{false};
};
