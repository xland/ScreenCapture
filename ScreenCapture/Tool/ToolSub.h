#pragma once

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "ToolBtn.h"


class StrokeCtrl;
class ColorCtrl;
class ToolSub : public QWidget
{
	Q_OBJECT

public:
	ToolSub(QWidget *parent = nullptr);
	~ToolSub();
	static void InitData(const QJsonObject& obj, const QString& lang);
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	static std::vector<ToolBtn> makeBtns(const QJsonArray& arr, const QString& lang);
private:
	qreal btnW{ 32 };
	StrokeCtrl* strokeCtrl;
	ColorCtrl* colorCtrl;
	int hoverIndex{ -1 };
};
