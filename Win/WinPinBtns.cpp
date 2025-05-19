#include <QMouseEvent>

#include "App/Util.h"
#include "Tool/ToolMain.h"
#include "WinPinBtns.h"
#include "WinBase.h"
#include "WinPin.h"

WinPinBtns::WinPinBtns(QWidget *parent) : QWidget(parent)
{
	setFixedSize(66, 30);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setMouseTracking(true);
	setCursor(Qt::PointingHandCursor);
	auto x = parent->width() - width();
	move(x, 0);
	show();
}

WinPinBtns::~WinPinBtns()
{

}

void WinPinBtns::paintEvent(QPaintEvent* event)
{
	QPainter p(this);
	p.setPen(Qt::NoPen);
	p.setBrush(QColor(0, 0, 0, 120));
	p.drawRect(rect());
	QRect r1(0, 0, 33, 30);
	QRect r2(33, 0, 33, 30);
	auto font = Util::getIconFont(13);
	p.setFont(*font);
	if (hoverIndex == 0)
	{
		p.setBrush(QColor(0, 0, 0, 220));
		p.drawRect(r1);
		p.setPen(QColor(255, 255, 255));
	}
	else if(hoverIndex == 1)
	{
		p.setBrush(QColor(0, 0, 0, 220));
		p.drawRect(r2);
		p.setPen(QColor(255, 255, 255));
	}
	else {
		p.setPen(QColor(255, 255, 255,188));
	}
	p.setBrush(Qt::NoBrush);
	p.drawText(r1, Qt::AlignCenter, QChar(0xe601));
	p.drawText(r2, Qt::AlignCenter, QChar(0xe6e7));
}

void WinPinBtns::mousePressEvent(QMouseEvent* event)
{
	auto winPin = (WinPin*)parent();
	if (hoverIndex == 0)
	{
		winPin->resetTool();
	}
	else if (hoverIndex == 1)
	{
		winPin->close();
	}
}

void WinPinBtns::mouseMoveEvent(QMouseEvent* event)
{
	int tempIndex{ -1 };
	if (event->pos().x() < width() / 2)
	{
		tempIndex = 0;
	}
	else
	{
		tempIndex = 1;
	}
	if (tempIndex != hoverIndex) {
		hoverIndex = tempIndex;
		update();
	}	
}

void WinPinBtns::mouseReleaseEvent(QMouseEvent* event)
{
}

void WinPinBtns::leaveEvent(QEvent* event)
{
	if (hoverIndex>=0)
	{
		hoverIndex = -1;
		update();
	}
}
