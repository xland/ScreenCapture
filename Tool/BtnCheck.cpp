#include <QPainter>
#include "../App/Font.h"
#include "ToolBase.h"
#include "BtnCheck.h"

BtnCheck::BtnCheck(const QString& name, const QChar& icon, const State& state, QWidget *parent) : BtnBase(name,icon,parent),
state{state}
{

}

BtnCheck::~BtnCheck()
{

}

void BtnCheck::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    auto r = rect().adjusted(2, 2, -2, -2);
    p.setPen(Qt::NoPen);
    auto font = Font::icon();
    font->setPixelSize(15);
    p.setFont(*font);
    if (isChecked)
    {
        p.setBrush(QColor(228, 238, 255));
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor(9, 88, 217));
        p.setBrush(Qt::NoBrush);
        p.drawText(r, Qt::AlignCenter, icon);
    }
    else if (isHover) {
        p.setBrush(QColor(228, 238, 255));
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor(33, 33, 33));
        p.setBrush(Qt::NoBrush);
        p.drawText(r, Qt::AlignCenter, icon);
    }
    else
    {
        p.setBrush(Qt::white);
        p.drawRoundedRect(r, 6, 6);
        p.setPen(QColor(33, 33, 33));
        p.setBrush(Qt::NoBrush);
        p.drawText(r, Qt::AlignCenter, icon);
    }
}

void BtnCheck::mousePressEvent(QMouseEvent* event)
{
    isChecked = !isChecked;
    update();
	ToolBase* toolBase = dynamic_cast<ToolBase*>(parent());
	toolBase->btnCheckChange(this);
}
