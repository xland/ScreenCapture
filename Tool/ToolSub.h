#pragma once
#include <QWidget>
#include "ToolBase.h"
#include "ToolBtn.h"

class StrokeCtrl;
class ColorCtrl;
class Box;
class ToolSub : public ToolBase
{
	Q_OBJECT

public:
	ToolSub(Box* box);
	~ToolSub();
	static void initData(const QJsonObject& obj, const QString& lang);
	bool getSelectState(const QString& btnName);
    QColor getColor();
    int getStrokeWidth();
public:
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
	static std::vector<ToolBtn> makeBtns(const QJsonArray& arr, const QString& lang);
private:
	StrokeCtrl* strokeCtrl;
	ColorCtrl* colorCtrl;
	int triangleX{ 0 };
};
