#include <QPainter>
#include "Btn.h"
#include "ToolBase.h"
#include "../App/Font.h"


Btn::Btn(const QString& name, const QChar& icon, QWidget *parent) : BtnBase(name,icon,parent)
{
    
}

Btn::~Btn()
{

}

void Btn::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    auto r = rect().adjusted(2, 2, -2, -2);
    p.setPen(Qt::NoPen);
    auto font = Font::icon();
    font->setPixelSize(15);
    p.setFont(*font);
    p.setBrush(isHover? QColor(228, 238, 255): Qt::white);
    p.drawRoundedRect(r, 6, 6);
    p.setPen(QColor(33, 33, 33));
    p.setBrush(Qt::NoBrush);
    p.drawText(r, Qt::AlignCenter, icon);
}

void Btn::mousePressEvent(QMouseEvent* event)
{
    ToolBase* toolBase = dynamic_cast<ToolBase*>(parent());
    toolBase->btnClick(this);
}
