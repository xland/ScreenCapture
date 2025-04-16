#include <windowsx.h>
#include <QWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QClipboard>

#include "WinPin.h"
#include "WinFull.h"
#include "WinMask.h"
#include "WinBoard.h"
#include "WinCanvas.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeTextContainer.h"
#include "../App/Util.h"
#include "../App/Lang.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "PixelInfo.h"


WinPin::WinPin(QWidget* parent) : WinBox(parent)
{
    padding = 8;
}

WinPin::~WinPin()
{
   
}

void WinPin::init(WinFull* full)
{
    auto winPin = new WinPin();
    winPin->prepareImg(full);
    winPin->x = full->x + full->rectMask.left() - winPin->padding;
    winPin->y = full->y + full->rectMask.top() - winPin->padding;
    winPin->w = winPin->imgBg.width();
    winPin->h = winPin->imgBg.height();
    winPin->initWindow();
    winPin->show();
    winPin->winBoard = new WinBoard(winPin);
    winPin->winCanvas = new WinCanvas(winPin);
    winPin->pixelInfo = new PixelInfo(winPin);
    full->close();
    SetFocus(winPin->hwnd);
}

void WinPin::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    QPoint pos{ x + padding, y + h };
    auto hwnd = (HWND)toolMain->winId();
    SetWindowPos(hwnd, nullptr, pos.x(), pos.y(), 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
    toolMain->setBtnEnable(QString{ "pin" }, false);
    toolMain->show();
    state = State::tool;
    if (pixelInfo) pixelInfo->close();
}

void WinPin::showToolSub()
{
    if (!toolSub) {
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
    QImage tar = imgBg.copy(QRect(padding, padding, w - padding * 2, h - padding * 2));
    QPainter p(&tar);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto shape : shapes)
    {
        shape->paint(&p);
    }
    Util::imgToClipboard(tar);
    close();

}

void WinPin::saveToFile()
{
    QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    auto filePath = QDir::cleanPath(desktopPath + QDir::separator() + "Img" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png");
    filePath = QFileDialog::getSaveFileName(toolMain, tr("保存文件"), filePath, "ScreenCapture (*.png)");
    if (filePath.isEmpty())
    {
        return;
    }
	QImage tar = imgBg.copy(QRect(padding, padding, w - padding * 2, h - padding * 2));
    QPainter p(&tar);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    for (auto shape : shapes)
    {
        shape->paint(&p);
    }
    tar.save(filePath);
    close();
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
    if (pixelInfo) {
        pixelInfo->close();
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
    if (pixelInfo)pixelInfo->hide();
    if (toolSub && toolSub->isVisible()) {
        mousePressOnShape(event);
    }
    else {
        if (toolMain) {
            needShowToolMain = toolMain->isVisible();
            if (needShowToolMain) {
                toolMain->hide();
            }
        }
        SetCapture(hwnd);
        posPress = event->position();
    }
}

void WinPin::mousePressRight(QMouseEvent* event)
{
    auto flag = state == State::start;
    HMENU hMenu = CreatePopupMenu();

    auto toolBar = Lang::get("toolBar").toStdWString();
    auto quit = Lang::get("quit").toStdWString();
    AppendMenu(hMenu, flag ? MF_UNCHECKED : MF_CHECKED, 1001, toolBar.data());
    AppendMenu(hMenu, MF_STRING, 1002, quit.data());
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
    saveToClipboard();
}

void WinPin::mouseMove(QMouseEvent* event)
{
    trackMouse();
    event->ignore();
    if (pixelInfo) {
        auto pos = event->pos();
        if (pos.x() > padding && pos.y() > padding && pos.x() < w - padding && pos.y() < h - padding) {            
            pixelInfo->mouseMove(pos);
        }
        else {
            pixelInfo->hide();
        }
    }
    if (state <= State::tool) {
        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        return;
    }
    mouseMoveOnShape(event);
    if (!event->isAccepted()) {
        if (state == State::text) {
            QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
        }
        else
        {
            QGuiApplication::setOverrideCursor(Qt::CrossCursor);
        }
    }
}

void WinPin::mouseDrag(QMouseEvent* event)
{
    event->ignore();
    mouseDragOnShape(event);
    if (GetCapture() == hwnd) {
        auto pos = event->globalPosition() - posPress;
        SetWindowPos(hwnd, NULL, pos.x(), pos.y(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        if (winBoard) {
            SetWindowPos(winBoard->hwnd, NULL, pos.x(), pos.y(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
        if (winCanvas) {
            SetWindowPos(winCanvas->hwnd, NULL, pos.x(), pos.y(), 0, 0, SWP_NOSIZE | SWP_NOZORDER);
        }
    }
}

void WinPin::mouseRelease(QMouseEvent* event)
{
    event->ignore();
    mouseReleaseOnShape(event);
    if (GetCapture() == hwnd) {
        ReleaseCapture();
        if (needShowToolMain) {
            showToolMain();
        }
        auto span = event->globalPosition() - posPress;
        for (int i = 0; i < shapes.size(); i++)
        {
            auto shapeText = qobject_cast<ShapeText*>(shapes[i]);
            if (shapeText) {
                auto hwnd = (HWND)shapeText->container->winId();
                auto xx = x + shapeText->container->ctrlRect.x();
                auto yy = y + shapeText->container->ctrlRect.y()-8;
                SetWindowPos(hwnd, nullptr, xx, yy, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
            }
        }
    }
}

void WinPin::escPress()
{
	close();
}

void WinPin::onMouseWheel(const int& delta)
{
    if (state == State::start) {
        float scale;
        if (delta > 0) {
            scale = 1.1f;
        }
        else {
            scale = 0.9f;
        }
        //imgRect.setXYWH(padding, padding, imgRect.width() * scale, imgRect.height() * scale);
        //auto tm = ToolMain::Get();
        //float w1, h1;
        //if (imgRect.width() < tm->ToolRect.width()) {
        //    w1 = tm->ToolRect.width() + padding * 2;
        //}
        //else {
        //    w1 = imgRect.width() + padding * 2;
        //}
        //h1 = imgRect.height() + padding * 2 + tm->ToolRect.height() * 2 + tm->MarginTop * 2;
        //x = x - (w1 - w) / 2;
        //y = y - (h1 - h) / 2;
        //w = w1;
        //h = h1;

        SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
        //DeleteDC(hCompatibleDC);
        //DeleteObject(bottomHbitmap);
        //HDC hdc = GetDC(hwnd);
        //hCompatibleDC = CreateCompatibleDC(NULL);
        //bottomHbitmap = CreateCompatibleBitmap(hdc, w, h);
        //DeleteObject(SelectObject(hCompatibleDC, bottomHbitmap));
        //ReleaseDC(hwnd, hdc);
        //initCanvas();
        //Refresh();
        return;
    }
    //Recorder::Get()->OnMouseWheel(delta);
    //return false;
}



void WinPin::moveByKey(const int& key)
{
    POINT point;
    GetCursorPos(&point);
    if (key == 0) SetCursorPos(point.x - 1, point.y);
    else if (key == 1) SetCursorPos(point.x, point.y - 1);
    else if (key == 2) SetCursorPos(point.x + 1, point.y);
    else if (key == 3) SetCursorPos(point.x, point.y + 1);
}

void WinPin::copyColor(const int& key)
{
    if (key == 3) {
        POINT pos;
        GetCursorPos(&pos);
        std::wstring tarStr = QString("%1,%2").arg(pos.x-x).arg(pos.y-y).toStdWString();
        Util::setClipboardText(tarStr);
        close();
        return;
    }
    Util::copyColor(key);
    close();
}

void WinPin::prepareImg(WinFull* full)
{
    auto tarImg = full->getCutImg();
    imgBg = QImage(tarImg.size() + QSize(padding * 2, padding * 2), QImage::Format_ARGB32_Premultiplied);
    imgBg.fill(Qt::transparent);
    QPainter p(&imgBg);
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
        QLinearGradient gradient(padding + tarImg.width(), 0, imgBg.width(), 0);
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
        QLinearGradient gradient(padding, padding+ tarImg.height(), padding, imgBg.height());
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
}

void WinPin::trackMouse()
{
    if (!isTrackMouseEvent) {
        TRACKMOUSEEVENT tme = {};
        tme.cbSize = sizeof(TRACKMOUSEEVENT);
        tme.dwFlags = TME_HOVER | TME_LEAVE;
        tme.hwndTrack = hwnd;
        tme.dwHoverTime = 1;
        isTrackMouseEvent = TrackMouseEvent(&tme);
    }
}

void WinPin::untrackMouse()
{
    TRACKMOUSEEVENT tme = {};
    tme.cbSize = sizeof(TRACKMOUSEEVENT);
    tme.dwFlags = TME_CANCEL | TME_HOVER | TME_LEAVE;
    tme.hwndTrack = hwnd;
    TrackMouseEvent(&tme);
    isTrackMouseEvent = false;
}

