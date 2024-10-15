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
	static void Init();
	static ToolSub* Get();
	static void Show();
	static void InitData(const QJsonObject& obj);
public:
	std::vector<ToolBtn> rectBtns;
	std::vector<ToolBtn> ellipseBtns;
	std::vector<ToolBtn> arrowBtns;
	std::vector<ToolBtn> numberBtns;
	std::vector<ToolBtn> penBtns;
	std::vector<ToolBtn> textBtns;
	std::vector<ToolBtn> mosaicBtns;
	std::vector<ToolBtn> eraserBtns;
	std::vector<ToolBtn> colorsBtns;
private:
};
