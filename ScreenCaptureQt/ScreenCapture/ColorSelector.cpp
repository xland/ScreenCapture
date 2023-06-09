#include "ColorSelector.h"
#include "ui_ColorSelector.h"
#include <QApplication>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>
#include <QColorDialog>

ColorSelector::ColorSelector(QWidget* parent) :
    QWidget(parent),
    ui(new Ui::ColorSelector)
{
    ui->setupUi(this);
    this->setMouseTracking(true);
    this->setCursor(Qt::ArrowCursor);
    this->hide();
    QColor cs[] =
    {
        QColor(0, 0, 255),
        QColor(0, 255, 0),
        QColor(255, 0, 0),
        QColor(0, 255, 255),
        QColor(255, 255, 0),
        QColor(255, 0, 255),

        QColor(255, 255, 255),
        QColor(128, 128, 128),
        QColor(0, 0, 0),
        QColor(128, 0, 0),
        QColor(128, 128, 0),
        QColor(0, 128, 0),
    };
    colors.append(Qt::red);
    rects.append(QRectF(6, 6, 20, 20));
    for (int var = 0; var < 6; ++var)
    {
        rects.append(QRectF(30 + var * 10 + var * 3, 6, 10, 8));
        colors.append(cs[var]);
        rects.append(QRectF(30 + var * 10 + var * 3, 17, 10, 9));
        colors.append(cs[11 - var]);
    }
}

ColorSelector::~ColorSelector()
{
    delete ui;
}

void ColorSelector::mouseMoveEvent(QMouseEvent* mouseEvent)
{
    auto mousePressPoint = mouseEvent->pos();
    for (int var = 0; var < 13; ++var)
    {
        if (rects[var].contains(mousePressPoint))
        {
            hoverIndex = var;
            repaint();
            return;
        }
    }
    hoverIndex = -1;
    repaint();
}
void ColorSelector::mousePressEvent(QMouseEvent* event)
{
    if (hoverIndex != -1)
    {
        if (hoverIndex == 0)
        {
            auto color = QColorDialog::getColor();
            if (color.isValid())
            {
                colors[0] = color;
            }
            return;
        }
        colors[0] = colors[hoverIndex];
        emit colorChanged();
        repaint();
    }
}
QColor ColorSelector::currentColor()
{
    return colors[0];
}

void ColorSelector::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QBrush(QColor(246, 246, 246)));
    painter.drawRect(this->rect());
    painter.setPen(QPen(QBrush(Qt::black), 0.5));
    for (int var = 0; var < 13; ++var)
    {
        painter.setBrush(QBrush(colors[var]));
        if (var == hoverIndex)
        {
            painter.drawRect(rects[var].x() - 1, rects[var].y() - 1, rects[var].width() + 2, rects[var].height() + 2);
        }
        else
        {
            painter.drawRect(rects[var]);
        }
    }

}
