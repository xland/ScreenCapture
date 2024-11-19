#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>
#include <QTextEdit>

#include "WinBase.h"


class CutMask;
class WinFull : public WinBase
{
	Q_OBJECT
public:
	WinFull(QWidget* parent = nullptr);
	~WinFull();
	static void init();
	static void dispose();
	static WinFull* get();
	void showToolMain() override;
	void showToolSub() override;
	void closeWin() override;
	void mousePress(QMouseEvent* event) override;
	void mouseMove(QMouseEvent* event) override;
	void mouseDrag(QMouseEvent* event) override;
	void mouseRelease(QMouseEvent* event) override;
public:
	CutMask* cutMask;
	float sf{ 1.0f };
	std::vector<QRect> screens;
protected:
	void paintEvent(QPaintEvent* event) override;
private:
};

