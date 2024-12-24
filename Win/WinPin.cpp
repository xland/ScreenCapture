#include "WinPin.h"
#include "WinFull.h"
#include "WinMask.h"

WinPin::WinPin(QObject* parent) : WinBox(parent)
{
	
}

WinPin::~WinPin()
{}

void WinPin::init(WinFull* full)
{
    auto winPin = new WinPin();
    winPin->img = prepareImg(full);
    winPin->x = full->x + full->winMask->maskRect.left() - 8;
    winPin->y = full->y + full->winMask->maskRect.top() - 8;
    winPin->w = winPin->img.width();
    winPin->h = winPin->img.height();
    winPin->initWindow(false);
    winPin->show();
    full->close();
}

void WinPin::showToolMain()
{
}

void WinPin::showToolSub()
{
}

void WinPin::saveToClipboard()
{
}

void WinPin::saveToFile()
{
}

void WinPin::close()
{
}

void WinPin::mousePress(QMouseEvent* event)
{
    event->ignore();
    mousePressOnShape(event);
}

void WinPin::mousePressRight(QMouseEvent* event)
{
}

void WinPin::mouseDBClick(QMouseEvent* event)
{
}

void WinPin::mouseMove(QMouseEvent* event)
{
    event->ignore();
    mouseMoveOnShape(event);
}

void WinPin::mouseDrag(QMouseEvent* event)
{
    event->ignore();
    mouseDragOnShape(event);
}

void WinPin::mouseRelease(QMouseEvent* event)
{
    event->ignore();
    mouseReleaseOnShape(event);
}

QImage WinPin::prepareImg(WinFull* full)
{
    auto padding{ 8 };
    auto tarImg = full->getCutImg();
    auto bgImg = QImage(tarImg.size() + QSize(padding * 2, padding * 2), QImage::Format_ARGB32_Premultiplied);
    bgImg.fill(Qt::transparent);
    QPainter p(&bgImg);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    {
        QRadialGradient gradient(QPointF(padding, padding), padding);
        gradient.setColorAt(0.0, QColor(88, 88, 88, 88));
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(0, 0, padding, padding);
    }
    {
        QLinearGradient gradient(0, 0, 0, padding);
        gradient.setColorAt(0.0, Qt::transparent);
        gradient.setColorAt(1.0, QColor(88, 88, 88, 88));
        p.setBrush(gradient);
        p.drawRect(padding, 0, tarImg.width(), padding);
    }
    {
        QRadialGradient gradient(QPointF(padding + tarImg.width(), padding), padding);
        gradient.setColorAt(0.0, QColor(88, 88, 88, 88));
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding + tarImg.width(), 0, padding, padding);
    }
    {
        QLinearGradient gradient(padding + tarImg.width(), 0, bgImg.width(), 0);
        gradient.setColorAt(0.0, QColor(88, 88, 88, 88));
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding+ tarImg.width(), padding, padding, tarImg.height());
    }
    {
        QRadialGradient gradient(QPointF(padding + tarImg.width(), padding + tarImg.height()), padding);
        gradient.setColorAt(0.0, QColor(88, 88, 88, 88));
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding + tarImg.width(), padding + tarImg.height(), padding, padding);
    }
    {
        QLinearGradient gradient(padding, padding+ tarImg.height(), padding, bgImg.height());
        gradient.setColorAt(0.0, QColor(88, 88, 88, 88));
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding, padding + tarImg.height(), tarImg.width(), padding);
    }
    {
        QRadialGradient gradient(QPointF(padding, padding + tarImg.height()), padding);
        gradient.setColorAt(0.0, QColor(88, 88, 88, 88));
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(0, padding + tarImg.height(), padding, padding);
    }
    {
        QLinearGradient gradient(0, 0, padding, 0);
        gradient.setColorAt(0.0, Qt::transparent);
        gradient.setColorAt(1.0, QColor(88, 88, 88, 88));
        p.setBrush(gradient);
        p.drawRect(0, padding, padding, tarImg.height());
    }
	p.drawImage(padding, padding, tarImg);
	return bgImg;
}

