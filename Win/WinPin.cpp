#include "WinPin.h"

WinPin::WinPin(QObject *parent) : WinBox(parent)
{}

WinPin::~WinPin()
{}

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

