#pragma once

#include <QWidget>
#include "WinBox.h"

class WinPin  : public WinBox
{
	Q_OBJECT

public:
	WinPin(const QImage&& img, QObject *parent = nullptr);
	~WinPin();
	static void init();
	void showToolMain() override;
	void showToolSub() override;
	void saveToClipboard() override;
	void saveToFile() override;
	void close() override;
protected:
	void mousePress(QMouseEvent* event) override;
	void mousePressRight(QMouseEvent* event) override;
	void mouseDBClick(QMouseEvent* event) override;
	void mouseMove(QMouseEvent* event) override;
	void mouseDrag(QMouseEvent* event) override;
	void mouseRelease(QMouseEvent* event) override;
};
