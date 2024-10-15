#pragma once

#include <QMainWindow>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "ToolBtn.h"

class ToolSub : public QMainWindow
{
	Q_OBJECT

public:
	ToolSub(QWidget *parent = nullptr);
	~ToolSub();
	static void InitData(const QJsonObject& obj, const QString& lang);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
	static ToolBtn makeBtn(const QJsonValue& val, const QString& lang);
private:
};
