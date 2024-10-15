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

class ToolMain : public QWidget
{
	Q_OBJECT

public:
	ToolMain(QWidget *parent = nullptr);
	~ToolMain();
	static void Init();
	static void InitData(const QJsonArray& arr,const QString& lang);
	static ToolMain* Get();
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	qreal btnW{ 32 };
	int hoverIndex{ -1 };
	int selectIndex{ -1 };
};
