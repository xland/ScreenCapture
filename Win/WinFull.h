#pragma once
#include "WinBase.h"


class CutMask;
class PixelInfo;

class WinFull : public WinBase
{
	Q_OBJECT
public:
	WinFull(QWidget* parent = nullptr);
	~WinFull();
	static void init();
	void close() override;
public:
	qreal dpr{ 1.0 };
	PixelInfo* pixelInfo;
	CutMask* cutMask;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
private:
};

