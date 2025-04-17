#pragma once
#include "WinBase.h"


class CutMask;
class ToolMain;
class ToolSub;
class PixelInfo;

class WinFull : public WinBase
{
	Q_OBJECT
public:
	WinFull(QWidget* parent = nullptr);
	~WinFull();
	static void init();
	//void showToolMain() override;
	//void showToolSub() override;
	void close() override;
	void saveToClipboard();
	void saveToFile();
	void moveByKey(const int& key);
	QImage getCutImg();
public:
	qreal dpr{ 1.0 };
	PixelInfo* pixelInfo;
	CutMask* cutMask;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	//void copyColor(const int& key) override;
	//void escPress() override;
private:
	void changeMouseState(const int& x, const int& y);
	void moveMaskRect(const QPoint& pos);
private:
};

