#pragma once
#include "WinBase.h"


class CutMask;
class WinLongTip;
class ToolLong;
class WinLong : public WinBase
{
	Q_OBJECT

public:
	WinLong(QWidget *parent = nullptr);
	~WinLong();
	void pin();
	CutMask* cutMask;
	WinLongTip* winLongTip;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
	QImage getTargetImg();
private:
	void initWindow();
	void startCap();
	void stopCap();
	void timerFunc();
	void capStep();
	void initTool();
	void initArea();
private:
	bool firstCheck{ true };
	QImage imgBg;
	int areaX, areaY, areaW, areaH;
	QImage img1;
	QImage img2;
	QImage imgResult;
	QTimer* stepTimer;
	HWND targetHwnd;
	ToolLong* tools;
	int dismissTime{ 0 };
};
