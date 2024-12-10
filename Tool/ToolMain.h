#pragma once

#include <QWidget>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "ToolBtn.h"

class WinBox;
class ToolMain : public QWidget
{
	Q_OBJECT
public:
	ToolMain(WinBox* win,QWidget *parent = nullptr);
	~ToolMain();
	static void InitData(const QJsonArray& arr,const QString& lang);
public:
	int selectIndex{ -1 };
	WinBox* win;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
private:
	int hoverIndex{ -1 };
	qreal btnW{ 32 };
};
