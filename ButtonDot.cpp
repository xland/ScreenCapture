#include "ButtonDot.h"
#include <QDebug>
#include <Icon.h>


ButtonDot::ButtonDot(QWidget* parent): QPushButton(parent)
{
    setCursor(Qt::PointingHandCursor);
    setMouseTracking(true);
    setMinimumSize(34, 32);
    setCheckable(true);
    setFlat(true);
    setAutoExclusive(true);
    setChecked(true);
    setFont(Icon::font);
    setText(Icon::icons[Icon::Name::dot]);
    setStyleSheet("font-size: " + QString::number(size + 10) + "px;");
}

void ButtonDot::wheelEvent(QWheelEvent* event)
{
    if (event->angleDelta().y() > 0)
    {
        size += 1;
    }
    else
    {
        size -= 1;
    }
    if (size > 58) size = 58;
    if (size < 1) size = 1;
    setStyleSheet("font-size: " + QString::number(size + 10) + "px;");
}

void ButtonDot::setFontSize(int size)
{
    this->size = size;
    setStyleSheet("font-size: " + QString::number(size + 10) + "px;");
}
