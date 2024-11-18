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
class ShapeBase;
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
public:
	int x, y, w, h;
	CutMask* cutMask;
	float scaleFactor{ 1.0f };
	std::vector<QRect> screens;
	std::vector<QRect> winRects;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void initWinRects();
	void initSize();
	void initBgImg();
	void initScreens();
private:
};

