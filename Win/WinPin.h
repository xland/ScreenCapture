#pragma once

#include <QWidget>
#include <QMenu>
#include <QAction>
#include "WinBase.h"

class WinFull;
class WinPin  : public WinBase
{
	Q_OBJECT

public:
	WinPin(QWidget *parent = nullptr);
	~WinPin();
	static void init(WinFull* full);
	void showToolMain();
	void showToolSub();
	void moveByKey(const int& key);
	void copyColor(const int& key);
protected:
	void mouseDBClick(QMouseEvent* event) override;
	void escPress();
private:
	void prepareImg(WinFull* full);
	void trackMouse();
	void untrackMouse();
	void onMouseWheel(const int& delta);
private:
	bool isTrackMouseEvent{ false };
	bool needShowToolMain{ false };
	QPointF posPress;
};
