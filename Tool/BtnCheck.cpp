#include <QPainter>

#include "../App/Util.h"
#include "ToolBase.h"
#include "BtnCheck.h"

BtnCheck::BtnCheck(const QString& name, const QChar& icon, QWidget* parent, State state, bool isChecked) : BtnBase(name, icon, parent),
state{state}, isChecked{ isChecked }
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
    auto font = Util::getIconFont(15);
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
    if (event->buttons() & Qt::LeftButton) {
        isChecked = !isChecked;
        update();
        ToolBase* toolBase = dynamic_cast<ToolBase*>(parent());
        toolBase->btnCheckChange(this);
    }
    else {
        qApp->exit(2);
    }
}
