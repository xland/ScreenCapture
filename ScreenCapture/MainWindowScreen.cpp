#include "MainWindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include "qdebug.h"
#include <qtimer.h>


void MainWindow::shotScreen()
{
    auto screens = QGuiApplication::screens();
    QList<QPixmap> pixmaps;
    qreal tempSize = 0;
    int x1 = 999999, y1 = 999999, x2 = -999999, y2 = -999999;
    for (size_t i = 0; i < screens.count(); i++)
    {
        auto p = screens[i]->grabWindow(0);
        pixmaps.append(std::move(p));
        auto r = screens[i]->devicePixelRatio();
        if (maxRate < r) {
            maxRate = r; 
        }
        auto rect = screens[i]->geometry();
        rect.setSize(rect.size() * screens[i]->devicePixelRatio()); 
        if (i != 0) {  
            if (rect.x() > screenRects[i - 1].right()) {
                rect.moveLeft(screenRects[i - 1].right());
            }
            if (rect.y() > screenRects[i - 1].bottom()) {
                rect.moveTop(screenRects[i - 1].bottom());
            }
        }
        if (rect.x() < x1) {
            x1 = rect.x();
        }
        if (rect.y() < y1) {
            y1 = rect.y();
        }
        if (rect.right() > x2) {
            x2 = rect.right();
        }
        if (rect.bottom() > y2) {
            y2 = rect.bottom();
        }
        screenRects.append(std::move(rect));
    }
    screenRect.adjust(x1, y1, x2, y2);
    desktopImage = new QImage(screenRect.size(), QImage::Format_ARGB32);
    desktopImage->setDevicePixelRatio(maxRate);
    QTransform transform;
    transform.translate(std::abs(screenRect.x()), std::abs(screenRect.y()));
    QPainter p(desktopImage);
    for (size_t i = 0; i < pixmaps.count(); i++)
    {
        pixmaps[i].setDevicePixelRatio(maxRate);
        auto pos = transform.map(screenRects[i].topLeft());
        p.drawPixmap(pos / maxRate, pixmaps[i]);
    }
    desktopImage->save("desktopImage.png");
}

void MainWindow::showWindowByRect(QRect desktopGeometry)
{
    //必须让它在抖动中show两次，不然有问题
    desktopGeometry.moveLeft(desktopGeometry.left() - 100);
    desktopGeometry.moveTop(desktopGeometry.top() - 100);
    setGeometry(desktopGeometry);
    QWidget::show();
    desktopGeometry.moveLeft(desktopGeometry.left() + 100);
    desktopGeometry.moveTop(desktopGeometry.top() + 100);
    setGeometry(desktopGeometry);
    QWidget::show();
    //QTimer::singleShot(1000, this, [this]() {
    //    auto desktopGeometry = QApplication::desktop()->geometry();
    //    });

}

void MainWindow::adjustWindowToScreen()
{
    auto screens = QGuiApplication::screens();
    auto count = screenRects.count();
    if (screens.count() > 1) {
        if (screenRects[1].left() > 0) {
            //副屏在主屏的右方
            if (std::abs(screenRects[0].right() - screenRects[1].left()) < 2) {
                //副屏在主屏的正右方
                auto size = screenRects[1].size();
                auto left = screenRects[1].left() / screens[1]->devicePixelRatio();
                //1920->125%->4->384*5;
                //1920->150%->3->640*3
                //1920->175%->2->823*2
                screenRect.moveLeft(823); //1920->125%->4->384;
                showWindowByRect(screenRect);
                //showWindowByRect(QApplication::desktop()->geometry());
            }
            else
            {
                auto span = (screenRects[1].right() - screenRects[0].right()) / maxRate;
                screenRect.moveLeft(200);
                showWindowByRect(screenRect);
            }
            
        }
        else
        {
            showWindowByRect(screenRect);
        }        
    }
    else
    {
        showWindowByRect(screenRect);
    }
    return;

    auto d1 = screens[0]->physicalDotsPerInch();
    auto d2 = screens[1]->physicalDotsPerInch();
    if (d1 < d2) {
        showWindowByRect(screenRect);
    }
    else
    {
        if (screens[0]->devicePixelRatio() > screens[1]->devicePixelRatio()) {
            showWindowByRect(QApplication::desktop()->geometry());
        }
        else
        {
            showWindowByRect(screenRect);
            //auto rect = QApplication::desktop()->geometry();
            //rect.setSize(rect.size() * 1.5);
            //showWindowByRect(rect);
        }
    }
}

void MainWindow::initWindowRects()
{
    EnumWindows([](HWND hwnd, LPARAM lparam)
        {
            if (!IsWindowVisible(hwnd)) return TRUE;
            if (IsIconic(hwnd)) return TRUE;
            if (GetWindowTextLength(hwnd) < 1) return TRUE;
            auto self = (MainWindow*)lparam;
            RECT rect;
            GetWindowRect(hwnd, &rect);
            QRect item;
            item.adjust(rect.left / self->maxRate, rect.top / self->maxRate, rect.right / self->maxRate, rect.bottom / self->maxRate);
            if (item.width() <= 6 || item.height() <= 6) {
                return TRUE;
            }
            self->windowRects.append(std::move(item));
            /*QPoint leftTop = self->transform.map(item.topLeft() / rate);
            item.moveTo(leftTop);
            item.setSize(item.size() / rate);
            instance->adjustRect2(item);*/
            //std::string title;
            //title.reserve(GetWindowTextLength(hwnd) + 1);
            //GetWindowTextA(hwnd, const_cast<CHAR*>(title.c_str()), title.capacity());            
            //OutputDebugStringA(title.c_str());
            //OutputDebugStringA("\r\n");            
            //QString a = QString::fromLatin1(title.data());
            //if (a.startsWith("asd.txt")) {
            //    QPainter p(self->desktopImage);
            //    p.setPen(QPen(QBrush(Qt::red), 3, Qt::SolidLine, Qt::RoundCap));
            //    p.setBrush(Qt::NoBrush);
            //    p.drawRect(item);
            //    //instance->desktopImage.save("desktopImage.png");
            //}

            return TRUE;
        }, (LPARAM)this);
}