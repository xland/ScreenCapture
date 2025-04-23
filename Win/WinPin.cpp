#include <windowsx.h>
#include <QWindow>
#include <QFileDialog>
#include <QStandardPaths>
#include <QApplication>
#include <QClipboard>

#include "WinPin.h"
#include "WinFull.h"
#include "Canvas.h"
#include "../Shape/ShapeBase.h"
#include "../Shape/ShapeText.h"
#include "../Shape/ShapeTextContainer.h"
#include "../App/Util.h"
#include "../App/Lang.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "PixelInfo.h"


WinPin::WinPin(QWidget* parent) : WinBase(parent)
{
}

WinPin::~WinPin()
{
   
}

void WinPin::init(WinFull* full)
{
    //auto winPin = new WinPin();
    //winPin->prepareImg(full);
    //winPin->x = full->x + full->rectMask.left() - winPin->padding;
    //winPin->y = full->y + full->rectMask.top() - winPin->padding;
    //winPin->w = winPin->imgBg.width();
    //winPin->h = winPin->imgBg.height();
    //winPin->initWindow();
    //winPin->show();
    ////winPin->winBoard = new WinBoard(winPin);
    //winPin->winCanvas = new WinCanvas(winPin);
    //winPin->pixelInfo = new PixelInfo(winPin);
    //full->close();
    //SetFocus(winPin->hwnd);
}

void WinPin::showToolMain()
{
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    QPoint pos{ x, y + h };
    auto hwnd = (HWND)toolMain->winId();
    SetWindowPos(hwnd, nullptr, pos.x(), pos.y(), 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_SHOWWINDOW);
    toolMain->setBtnEnable(QString{ "pin" }, false);
    toolMain->show();
    state = State::tool;
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





void WinPin::mouseDBClick(QMouseEvent* event)
{
    saveToClipboard();
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
 //   auto tarImg = full->getCutImg();
 //   imgBg = QImage(tarImg.size() + QSize(padding * 2, padding * 2), QImage::Format_ARGB32_Premultiplied);
 //   imgBg.fill(Qt::transparent);    
	//p.drawImage(padding, padding, tarImg);
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

