#include "MainWindow.h"
#include <QScreen>
#include <QGuiApplication>
#include <Windows.h>
#include <qtransform.h>
#include <qapplication.h>
#include <qdesktopwidget.h>
#include <qwindow.h>
#include "qdebug.h"


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
    //desktopImage->save("desktopImage.png");
}



void MainWindow::adjustWindowToScreen()
{
    auto desktopGeometry = QApplication::desktop()->geometry();
    //desktopGeometry.moveLeft(desktopGeometry.left() - 100);
    //desktopGeometry.moveTop(desktopGeometry.top() - 100);
    //desktopGeometry.moveLeft(desktopGeometry.left() + 100);
    //desktopGeometry.moveTop(desktopGeometry.top() + 100);
    setGeometry(desktopGeometry);
    QWidget::show();
    setGeometry(desktopGeometry);
    QWidget::show();

    auto screens = QGuiApplication::screens();
    if (screens[0]->devicePixelRatio() > screens[1]->devicePixelRatio() || screenRects[0].size() > screenRects[1].size()) {
        //如果主屏的缩放比例，比副屏高
        paintPosition.setX(0);
        paintPosition.setY(0);
        return;
    }


    if (std::abs(screenRects[0].right() - screenRects[1].left()) < 4) {
        //主屏在左侧
        auto span = screenRects[0].width() / (maxRate * 2);
        if (span > 640) span = span / 2;
        span = 0;
        auto x = screenRects[0].x() + span;
        paintPosition.setX(x);
        if (screenRects[0].top() < screenRects[1].top()) {
            //主屏在左侧上方
            span = screenRects[1].top() - screenRects[0].top();//594
            span = span/ (maxRate * 2);
            paintPosition.setY(screenRects[0].y()+span);
        }
        else
        {
            paintPosition.setY(screenRects[0].y());
        }
    }
    
    
}