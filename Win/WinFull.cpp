#include <QApplication>
#include <QPainter>
#include <QClipboard>
#include <QFileDialog>
#include <QStandardPaths>
#include <QPainterPath>
#include <dwmapi.h>

#include "../App/App.h"
#include "../App/NativeRect.h"
#include "../Shape/ShapeBase.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../App/Util.h"
#include "WinFull.h"
#include "WinPin.h"
#include "CutMask.h"
#include "PixelInfo.h"

namespace {
    WinFull* winFull;
}

WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    x = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y = GetSystemMetrics(SM_YVIRTUALSCREEN);
    w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    h = GetSystemMetrics(SM_CYVIRTUALSCREEN);    
    initWindow();
    cutMask = new CutMask(this);
    auto dpr = devicePixelRatio();
    imgBg = Util::printScreen();
    imgBg.setDevicePixelRatio(dpr);
    imgBoard = imgBg.copy();
    imgCanvas = imgBg.copy();
    pixelInfo = new PixelInfo(this);
    pixelInfo->mouseMove(QCursor::pos());
}
WinFull::~WinFull()
{   
}
void WinFull::init()
{
    winFull = new WinFull();
}

void WinFull::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.drawImage(0, 0, imgBg);
    p.drawImage(0, 0, imgBoard);
    p.drawImage(0, 0, imgCanvas);
    cutMask->paint(p);
}


void WinFull::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        posPress = event->pos();
        if (state < State::mask)
        {
            if (pixelInfo) pixelInfo->close();
        }
        else if (state == State::mask) {
            cutMask->mousePress(event);
        }
        else if (state >= State::tool) {

        }
    }
}

void WinFull::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        
        if (toolMain && toolMain->isVisible()) {
            toolMain->hide();
        }
        if (toolSub && toolSub->isVisible()) {
            toolSub->hide();
        }
        cutMask->mouseDrag(event);
    }
    else {
        cutMask->mouseMove(event);
    }
}

void WinFull::mouseReleaseEvent(QMouseEvent* event)
{
    state = State::tool;
    rectMask = rectMask.normalized();
    update();
    auto br = rectMask.bottomRight();
    if (!toolMain) {
        toolMain = new ToolMain(this);
    }
    toolMain->confirmPos();
    toolMain->show();
}

//void WinFull::showToolMain()
//{
    //if (!toolMain) {
    //    toolMain = new ToolMain(this);
    //}
    //toolMain->setBtnEnable(QString{ "pin" }, true);
    //toolMain->confirmPos();
    //toolMain->show();
//}


//void WinFull::showToolSub()
//{
    //if (!toolSub) {
    //    toolSub = new ToolSub(this);
    //}
    //if (toolSub->isVisible()) {
    //    toolSub->hide();
    //}
    //toolSub->show();
//}

void WinFull::close()
{
    deleteLater();
    winFull = nullptr;
}

QImage WinFull::getCutImg()
{
 //   QImage tar = img.copy(winMask->maskRect);
 //   QPainter p(&tar);
 //   p.translate(0 - winMask->maskRect.left(), 0 - winMask->maskRect.top());
 //   p.setRenderHint(QPainter::Antialiasing, true);
 //   p.setRenderHint(QPainter::TextAntialiasing, true);
 //   for (auto shape : shapes)
 //   {
 //       shape->paint(&p);
 //   }
	//return tar; 
    return QImage();
}

void WinFull::saveToClipboard()
{
    //Util::imgToClipboard(getCutImg());
    //close();
}

void WinFull::saveToFile()
{
    //QString desktopPath = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    //auto filePath = QDir::cleanPath(desktopPath + QDir::separator() + "Img" + QDateTime::currentDateTime().toString("yyyyMMddhhmmsszzz") + ".png");
    //filePath = QFileDialog::getSaveFileName(toolMain, tr("保存文件"), filePath, "ScreenCapture (*.png)");
    //if (filePath.isEmpty())
    //{
    //    return;
    //}
    //getCutImg().save(filePath);
    //close();
}

void WinFull::moveByKey(const int& key)
{
    //if (state == State::start) {
    //    POINT point;
    //    GetCursorPos(&point);
    //    if (key == 0) SetCursorPos(point.x - 1, point.y);
    //    else if (key == 1) SetCursorPos(point.x, point.y - 1);
    //    else if (key == 2) SetCursorPos(point.x + 1, point.y);
    //    else if (key == 3) SetCursorPos(point.x, point.y + 1);
    //}
    //else {
    //    SetFocus(hwnd);
    //    hideTools(state);
    //    if (key == 0) winMask->maskRect.translate(-1, 0);
    //    else if (key == 1) winMask->maskRect.translate(0, -1);
    //    else if (key == 2) winMask->maskRect.translate(1, 0);
    //    else if (key == 3) winMask->maskRect.translate(0, 1);
    //    showToolMain();
    //    winMask->update();
    //}
}
//void WinFull::copyColor(const int& key)
//{
    //if (key == 3) {
    //    POINT pos;
    //    GetCursorPos(&pos);
    //    std::wstring tarStr = QString("%1,%2").arg(pos.x).arg(pos.y).toStdWString();
    //    Util::setClipboardText(tarStr);
    //    close();
    //    return;
    //}
    //winMask->close();
    //Util::copyColor(key);
    //close();
//}

//void WinFull::escPress()
//{
	//if (state > State::start) {
 //       hideTools();
 //       SetFocus(hwnd);
	//}
 //   else {
 //       close();
 //   }
//}

void WinFull::changeMouseState(const int& x, const int& y)
{
    
}