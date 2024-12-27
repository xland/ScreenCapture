#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "WinBox.h"

class WinFull;
class WinPin  : public WinBox
{
	Q_OBJECT

public:
	WinPin(QObject *parent = nullptr);
	~WinPin();
	static void initData(const QJsonArray& arr, const QString& lang);
	static void init(WinFull* full);
	void showToolMain() override;
	void showToolSub() override;
	void saveToClipboard() override;
	void saveToFile() override;
	void close() override;
	void ctrlTPress() override;
protected:
	void mousePress(QMouseEvent* event) override;
	void mousePressRight(QMouseEvent* event) override;
	void mouseDBClick(QMouseEvent* event) override;
	void mouseMove(QMouseEvent* event) override;
	void mouseDrag(QMouseEvent* event) override;
	void mouseRelease(QMouseEvent* event) override;
private:
	void prepareImg(WinFull* full);
private:
	bool needShowToolMain{ false };
	QPoint posPress;
};
