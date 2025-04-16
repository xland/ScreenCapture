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
#include "WinMask.h"
#include "WinCanvas.h"
#include "PixelInfo.h"

WinFull::WinFull(QWidget* parent) : WinBase(parent)
{
    auto rect = NativeRect::getDesktopRect();
    x = rect.x();
    y = rect.y();
    w = rect.width();
    h = rect.height();
    initWinRect();
    initWindow();
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
    auto winFull = new WinFull();
}

void WinFull::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    p.drawImage(0, 0, imgBg);
    p.drawImage(0, 0, imgBoard);
    p.drawImage(0, 0, imgCanvas);

    p.setBrush(QColor(0, 0, 0, 120));
    QColor borderColor(22, 118, 255);
    p.setPen(QPen(QBrush(borderColor), 1.5));
    QPainterPath path;
    path.addRect(-1, -1, w + 1, h + 1);
    path.addRect(rectMask);
    p.drawPath(path);
    if (state != State::mask) return;
    if (QApplication::mouseButtons() & Qt::LeftButton) return;

    //绘制边框上的拖动圆点
    p.setBrush(borderColor);
    p.setPen(QPen(QBrush(QColor(255, 255, 255)), 1));
    auto r{ 3 };
    auto hw{ rectMask.width() / 2 };
    auto hh{ rectMask.height() / 2 };
    p.drawEllipse(rectMask.topLeft().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.left() + hw, rectMask.top()), r, r);
    p.drawEllipse(rectMask.topRight().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.right(), rectMask.top() + hh), r, r);
    p.drawEllipse(rectMask.bottomRight().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.left() + hw, rectMask.bottom()), r, r);
    p.drawEllipse(rectMask.bottomLeft().toPointF(), r, r);
    p.drawEllipse(QPointF(rectMask.left(), rectMask.top() + hh), r, r);

    //绘制截图区域位置和大小
    auto text = QString("X:%1 Y:%2 R:%3 B:%4 W:%5 H:%6")
        .arg(rectMask.x()).arg(rectMask.y())
        .arg(rectMask.right()).arg(rectMask.bottom())
        .arg(rectMask.width()).arg(rectMask.height());
    auto font = Util::getTextFont(10);
    QFontMetrics fm(*font);
    p.setFont(*font);
    int w = fm.horizontalAdvance(text);
    if (y < 0) y = rectMask.y() + 4;
    QRect rect(rectMask.x(), rectMask.y() - 25, w + 14, 22);
    p.setBrush(QColor(0, 0, 0, 120));
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(rect, 3, 3);
    p.setPen(QPen(QBrush(Qt::white), 1));
    p.setBrush(Qt::NoBrush);
    p.drawText(rect, Qt::AlignCenter, text);
}


void WinFull::initWinRect()
{
    rectWins.clear();
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!hwnd) return TRUE;
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            RECT rect;
            DwmGetWindowAttribute(hwnd, DWMWA_EXTENDED_FRAME_BOUNDS, &rect, sizeof(RECT));
            if (rect.right - rect.left <= 6 || rect.bottom - rect.top <= 6) {
                return TRUE;
            }
            auto self = (WinFull*)lparam;
            if (rect.left < self->x) rect.left = self->x;
            if (rect.top < self->y) rect.top = self->y;
            if (rect.right > self->x + self->w) rect.right = self->x + self->w;
            if (rect.bottom > self->y + self->h) rect.bottom = self->y + self->h;
            auto lt = Util::getQtPoint(rect.left, rect.top);
            auto rb = Util::getQtPoint(rect.right, rect.bottom);
            lt = self->mapFromGlobal(lt);
            rb = self->mapFromGlobal(rb);
            self->rectWins.push_back(QRect(lt, rb));
            return TRUE;
        }, (LPARAM)this);
}


void WinFull::mousePressEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        posPress = event->pos();
        if (pixelInfo) {
            pixelInfo->hide();
        }
        if (state == State::mask) {
            changeRectMask(posPress);
            update();
        }
        else if (state >= State::tool) {

        }
    }
}

void WinFull::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton) {
        auto pos = event->pos();
        if (toolMain && toolMain->isVisible()) {
            toolMain->hide();
        }
        if (toolSub && toolSub->isVisible()) {
            toolSub->hide();
        }
        if (state == State::start) {
            if (pos == posPress) return;
            rectMask.setCoords(posPress.x(), posPress.y(), pos.x(), pos.y());
            update();
        }
        else if (state == State::tool) {
            if (mouseState == 0)
            {
                moveMaskRect(pos);
            }
            else {
                changeRectMask(pos);
            }
            update();
        }
    }
    else {
        auto pos = event->pos();
        if (state == State::start) {
            pixelInfo->mouseMove(pos);
            for (const auto& rect : rectWins)
            {
                if (rect.contains(pos)) {
                    if (rectMask == rect) return;
                    rectMask = rect;
                    update();
                    return;
                }
            }
        }
        else if (state <= State::tool) {
            changeMouseState(pos.x(), pos.y());
        }
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
    //if (pixelInfo) {
    //    pixelInfo->close();
    //}
    //if (winMask) {
    //    winMask->close();
    //}
    //if (winCanvas) {
    //    winCanvas->close();
    //}
    //if (winBoard) {
    //    winBoard->close();
    //}
    //if (toolMain) {
    //    toolMain->close();
    //}
    //if (toolSub) {
    //    toolSub->close();
    //}
    //WinBase::close();
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
    auto leftX = rectMask.left(); auto topY = rectMask.top();
    auto rightX = rectMask.right(); auto bottomY = rectMask.bottom();
    if (x > leftX + 1 && y > topY + 1 && x < rightX - 1 && y < bottomY - 1) //不然截图区域上顶天，下顶地的时候没法改变高度
    {
        setCursor(Qt::SizeAllCursor);
        mouseState = 0;
    }
    else if (x <= leftX && y <= topY)
    {
        setCursor(Qt::SizeFDiagCursor);
        mouseState = 1;
    }
    else if (x >= leftX && x <= rightX && y <= topY)
    {
        setCursor(Qt::SizeVerCursor);
        mouseState = 2;
    }
    else if (x >= rightX && y <= topY)
    {
        setCursor(Qt::SizeBDiagCursor);
        mouseState = 3;
    }
    else if (x >= rightX && y >= topY && y <= bottomY)
    {
        setCursor(Qt::SizeHorCursor);
        mouseState = 4;
    }
    else if (x >= rightX && y >= bottomY)
    {
        setCursor(Qt::SizeFDiagCursor);
        mouseState = 5;
    }
    else if (x >= leftX && x <= rightX && y >= bottomY)
    {
        setCursor(Qt::SizeVerCursor);
        mouseState = 6;
    }
    else if (x <= leftX && y >= bottomY)
    {
        setCursor(Qt::SizeBDiagCursor);
        mouseState = 7;
    }
    else if (x <= leftX && y <= bottomY && y >= topY)
    {
        setCursor(Qt::SizeHorCursor);
        mouseState = 8;
    }
}

void WinFull::changeRectMask(const QPoint& pos)
{
    if (mouseState == 1)
    {
        rectMask.setTopLeft(pos);
    }
    else if (mouseState == 2)
    {
        rectMask.setTop(pos.y());
    }
    else if (mouseState == 3)
    {
        rectMask.setTopRight(pos);
    }
    else if (mouseState == 4)
    {
        rectMask.setRight(pos.x());
    }
    else if (mouseState == 5)
    {
        rectMask.setBottomRight(pos);
    }
    else if (mouseState == 6)
    {
        rectMask.setBottom(pos.y());
    }
    else if (mouseState == 7)
    {
        rectMask.setBottomLeft(pos);
    }
    else if (mouseState == 8)
    {
        rectMask.setLeft(pos.x());
    }
}
void WinFull::moveMaskRect(const QPoint& pos)
{
    auto span = pos - posPress;
    posPress = pos;
    auto target = rectMask.topLeft() + span;
    if (target.x() < 0) {
        target.setX(0);
        rectMask.moveTo(target);
        return;
    }
    if (target.y() < 0) {
        target.setY(0);
        rectMask.moveTo(target);
        return;
    }
    if (target.x() + rectMask.width() > width()) {
        target.setX(width() - rectMask.width());
        rectMask.moveTo(target);
        return;
    }
    if (target.y() + rectMask.height() > height()) {
        target.setY(height() - rectMask.height());
        rectMask.moveTo(target);
        return;
    }
    rectMask.moveTo(target);
}