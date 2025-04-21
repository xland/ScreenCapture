#include <QToolTip>
#include "../App/Lang.h"
#include "BtnBase.h"

BtnBase::BtnBase(const QString& name, const QChar& icon, QWidget *parent) : QWidget(parent),
name{name},icon{icon}
{
	setMouseTracking(true);
	setAttribute(Qt::WA_Hover);
	setCursor(Qt::PointingHandCursor);
	setFocusPolicy(Qt::StrongFocus);
	setFixedSize(32, 28);
}

BtnBase::~BtnBase()
{
}

void BtnBase::enterEvent(QEnterEvent* event)
{
	if (!isHover)
	{
		auto tip = Lang::get(name);
		QToolTip::showText(QCursor::pos(), tip, this);
	}
	isHover = true;
	update();
}

void BtnBase::leaveEvent(QEvent* event)
{
	isHover = false;
	update();
}
