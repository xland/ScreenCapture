#include <QApplication>
#include <Windows.h>
#include <qscreen.h>
#include <qimage.h>
#include <qwindow.h>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>

#include "WinFull.h"
#include "WinPin.h"
#include "WinMask.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "../Shape/ShapeBase.h"
#include "../App/App.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/PixelInfo.h"
#include "../App/Util.h"

WinFull::WinFull(QObject* parent) : WinBox(parent)
{
    initWinSizeByDesktopSize();
    img = Util::printScreen(x, y, w, h);
    initWindow(false);
    show();
    releaseImg();
}
WinFull::~WinFull()
{
    
}
void WinFull::init()
{
    auto winFull = new WinFull();
    winFull->winMask = new WinMask(winFull);
    winFull->pixelInfo = new PixelInfo(winFull);
}
void WinFull::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain(this);
    } 
    auto pos = winMask->maskRect.bottomRight();
    pos.setX(pos.x() + x);
    pos.setY(pos.y() + y);
    auto hwnd = (HWND)toolMain->winId();
    auto dpr = toolMain->windowHandle()->devicePixelRatio();
    SetWindowPos(hwnd, nullptr,pos.x() - toolMain->width()*dpr, pos.y() + 6*dpr, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
    toolMain->show();
    toolMain->raise();
}
void WinFull::showToolSub()
{
    if (!toolSub) {
        winBoard = new WinBoard(this);
        winCanvas = new WinCanvas(this);
        winMask->raise();  //todo 确认会不会闪一下
        toolMain->raise();
        toolSub = new ToolSub(this);
    }
    if (toolSub->isVisible()) {
        toolSub->hide();
    }
    toolSub->show();
    toolSub->raise();
}

void WinFull::initWinSizeByDesktopSize()
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
}

void WinFull::close()
{
    if (pixelInfo) {
        pixelInfo->close();
    }
    if (winMask) {
        winMask->close();
    }
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

QImage WinFull::getCutImg()
{
    QImage tar;
    {
        auto img = Util::printWindow(this);
        tar = img.copy(winMask->maskRect);
    }
    QPainter p(&tar);
    p.translate(0 - winMask->maskRect.left(), 0 - winMask->maskRect.top());
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto shape : shapes)
    {
        shape->paint(&p);
    }
	return tar; 
}

void WinFull::saveToClipboard()
{
    Util::imgToClipboard(getCutImg());
    close();
}

void WinFull::saveToFile()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    auto filePath = QDir::cleanPath(desktopPath + QDir::separator() + "Img" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png");
    filePath = QFileDialog::getSaveFileName(toolMain, tr("保存文件"), filePath, "ScreenCapture (*.png)");
    if (filePath.isEmpty())
    {
        return;
    }
    getCutImg().save(filePath);
    close();
}

void WinFull::mousePress(QMouseEvent* event)
{
    event->ignore();
    winMask->mousePress(event);
    mousePressOnShape(event);
}

void WinFull::mouseMove(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseMove(event);
    if (pixelInfo && !event->isAccepted()) {
        pixelInfo->mouseMove(event);
    }
    if (!event->isAccepted()) {
        mouseMoveOnShape(event);
    }
    if (!event->isAccepted()) {
        if (state == State::text) {
            QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
        }
        else {
            QGuiApplication::setOverrideCursor(Qt::CrossCursor);
        }
    }
}

void WinFull::mouseDrag(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseDrag(event);
    mouseDragOnShape(event);
}

void WinFull::mouseRelease(QMouseEvent* event)
{
    event->ignore();
    winMask->mouseRelease(event);
    mouseReleaseOnShape(event);
}