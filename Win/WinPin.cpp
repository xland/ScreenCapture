#include <QWindow>

#include "WinPin.h"
#include "WinFull.h"
#include "WinMask.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"

namespace{
    static int padding{ 8 };
}

WinPin::WinPin(QObject* parent) : WinBox(parent)
{

}

WinPin::~WinPin()
{
   
}

void WinPin::init(WinFull* full)
{
    auto winPin = new WinPin();
    winPin->img = prepareImg(full);
    winPin->x = full->x + full->winMask->maskRect.left() - padding;
    winPin->y = full->y + full->winMask->maskRect.top() - padding;
    winPin->w = winPin->img.width();
    winPin->h = winPin->img.height();
    winPin->initWindow(false);
    winPin->show();
    full->close();
}

void WinPin::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    QPoint pos{ x + padding, y + h };
    auto hwnd = (HWND)toolMain->winId();
    auto dpr = toolMain->windowHandle()->devicePixelRatio();
    SetWindowPos(hwnd, nullptr, pos.x(), pos.y() + 6 * dpr, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
    toolMain->show();
    state = State::tool;
}

void WinPin::showToolSub()
{
    if (!toolSub) {
        winBoard = new WinBoard(this);
        winCanvas = new WinCanvas(this);
        toolMain->raise();
        toolSub = new ToolSub(this);
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    toolSub->show();
    toolSub->raise();
}

void WinPin::saveToClipboard()
{
}

void WinPin::saveToFile()
{
}

void WinPin::close()
{
    if (winCanvas) {
        winCanvas->close();
    }
    if (winBoard) {
        winBoard->close();
    }
    if (toolMain) {
        toolMain->close();
    }
    if (toolSub) {
        toolSub->close();
    }
    WinBase::close();
}

void WinPin::ctrlTPress()
{
	if (state == State::start)
	{
		showToolMain();
	}
	else
	{
        hideTools();
	}
}

void WinPin::mousePress(QMouseEvent* event)
{
    event->ignore();
    if (toolSub && toolSub->isVisible()) {

    }
    else {
        if (toolMain) {
            needShowToolMain = toolMain->isVisible();
            if (needShowToolMain) {
                toolMain->hide();
            }
        }
        SetCapture(hwnd);
        posPress = event->pos();
    }
}

void WinPin::mousePressRight(QMouseEvent* event)
{
    HMENU hMenu = CreatePopupMenu();
    auto flag = state == State::start;
    AppendMenu(hMenu, flag ? MF_UNCHECKED : MF_CHECKED, 1001, L"工具栏（Ctrl+T）");
    AppendMenu(hMenu, MF_STRING, 1002, L"退出（Esc）");
    auto pos = event->pos();
    int cmd = TrackPopupMenu(hMenu, TPM_RETURNCMD | TPM_NONOTIFY, x+pos.x(), y+pos.y(), 0, hwnd, NULL);
    if (cmd == 1001) {
       if (state == State::start)
       {
           showToolMain();
           state = State::tool;
	   }
       else
       {
           hideTools();
           state = State::start;
       }
    }
    else if (cmd == 1002) {
        close();
    }
    DestroyMenu(hMenu);
}

void WinPin::mouseDBClick(QMouseEvent* event)
{
}

void WinPin::mouseMove(QMouseEvent* event)
{
    event->ignore();
    //mouseMoveOnShape(event);
}

void WinPin::mouseDrag(QMouseEvent* event)
{
    event->ignore();
    //mouseDragOnShape(event);
    if (GetCapture() == hwnd) {
        auto span = event->pos() - posPress;
        SetWindowPos(hwnd, NULL, x + span.x(), y + span.y(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
    }
}

void WinPin::mouseRelease(QMouseEvent* event)
{
    event->ignore();
    //mouseReleaseOnShape(event);
    if (GetCapture() == hwnd) {
        ReleaseCapture();
        if (needShowToolMain) {
            showToolMain();
        }
    }
}

QImage WinPin::prepareImg(WinFull* full)
{
    auto tarImg = full->getCutImg();
    auto bgImg = QImage(tarImg.size() + QSize(padding * 2, padding * 2), QImage::Format_ARGB32_Premultiplied);
    bgImg.fill(Qt::transparent);
    QPainter p(&bgImg);
    p.setRenderHint(QPainter::Antialiasing);
    p.setPen(Qt::NoPen);
    QColor c(88, 88, 88, 88);
    {
        QRadialGradient gradient(QPointF(padding, padding), padding);
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(0, 0, padding, padding);
    }
    {
        QLinearGradient gradient(0, 0, 0, padding);
        gradient.setColorAt(0.0, Qt::transparent);
        gradient.setColorAt(1.0, c);
        p.setBrush(gradient);
        p.drawRect(padding, 0, tarImg.width(), padding);
    }
    {
        QRadialGradient gradient(QPointF(padding + tarImg.width(), padding), padding);
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding + tarImg.width(), 0, padding, padding);
    }
    {
        QLinearGradient gradient(padding + tarImg.width(), 0, bgImg.width(), 0);
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding+ tarImg.width(), padding, padding, tarImg.height());
    }
    {
        QRadialGradient gradient(QPointF(padding + tarImg.width(), padding + tarImg.height()), padding);
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding + tarImg.width(), padding + tarImg.height(), padding, padding);
    }
    {
        QLinearGradient gradient(padding, padding+ tarImg.height(), padding, bgImg.height());
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(padding, padding + tarImg.height(), tarImg.width(), padding);
    }
    {
        QRadialGradient gradient(QPointF(padding, padding + tarImg.height()), padding);
        gradient.setColorAt(0.0, c);
        gradient.setColorAt(1.0, Qt::transparent);
        p.setBrush(gradient);
        p.drawRect(0, padding + tarImg.height(), padding, padding);
    }
    {
        QLinearGradient gradient(0, 0, padding, 0);
        gradient.setColorAt(0.0, Qt::transparent);
        gradient.setColorAt(1.0, c);
        p.setBrush(gradient);
        p.drawRect(0, padding, padding, tarImg.height());
    }
	p.drawImage(padding, padding, tarImg);
	return bgImg;
}

