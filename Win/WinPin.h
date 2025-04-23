#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "WinBase.h"

class WinPin  : public WinBase
{
	Q_OBJECT

public:
	WinPin(QWidget *parent = nullptr);
	~WinPin();
	static void init();
	void showToolMain();
	void showToolSub();
	void moveByKey(const int& key);
	void copyColor(const int& key);
protected:
	void mouseDBClick(QMouseEvent* event) override;
	void escPress();
private:
	void prepareImg();
	void trackMouse();
	void untrackMouse();
	void onMouseWheel(const int& delta);
private:
	bool isTrackMouseEvent{ false };
	bool needShowToolMain{ false };
	QPointF posPress;
};
