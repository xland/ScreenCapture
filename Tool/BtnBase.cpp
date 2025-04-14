#include "BtnBase.h"

BtnBase::BtnBase(const QString& name, const QChar& icon, QWidget *parent) : QWidget(parent),
name{name},icon{icon}
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover);
	setCursor(Qt::PointingHandCursor);
}

BtnBase::~BtnBase()
{
}

void BtnBase::enterEvent(QEnterEvent* event)
{
	isHover = true;
	update();
}

void BtnBase::leaveEvent(QEvent* event)
{
	isHover = false;
	update();
}
