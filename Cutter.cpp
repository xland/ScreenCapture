
#include "Cutter.h"
#include <QCursor>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <cmath>
#include <QDebug>


static Cutter* instance;

Cutter::Cutter(QObject *parent)
    : QObject{parent}
{
    createCutAreaByWindowRect();
}
Cutter::~Cutter()
{

}
void Cutter::Init(QObject *parent)
{
    instance = new Cutter(parent);
}
Cutter* Cutter::Get()
{
    return instance;
}
void Cutter::copyColor(bool isRgb)
{
    QClipboard* clipboard = QApplication::clipboard();
    if(isRgb){
        clipboard->setText(colorRgb);
    }else{
        clipboard->setText(colorHex);
    }
}
void Cutter::moveMousePosition(int type)
{
    auto position = QCursor::pos();
    if(type == 0){
        auto x = position.x() -1;
        if(x < 0) x = 0;
        QCursor::setPos(x,position.y());
    }else if(type == 1){
        auto y = position.y() -1;
        if(y < 0) y = 0;
        QCursor::setPos(position.x(),y);
    }else if(type == 2){
        //todo
        QCursor::setPos(position.x()+1,position.y());
    }else if(type == 3){
        //todo
        QCursor::setPos(position.x(),position.y()+1);
    }
}
QVariant Cutter::getArrowPoints(QPointF start,QPointF end,qreal width,qreal height)
{
    QVariantList list;
    //箭头起点和箭头终点组成的线段，这个线段与水平坐标的夹角
    QLineF tarLine (start,end);
    //角度变弧度，以适应std::cos，std::sin
    qreal v = tarLine.angle()*3.14159265/180;
    // △底边的中点
    qreal centerX = end.x() - height * std::cos(v);
    qreal centerY = end.y() + height * std::sin(v);
    qreal tempA = width/4*std::sin(v);
    qreal tempB = width/4*std::cos(v);
    // △ 左下的顶点与底边中点之间中间位置的点
    qreal x1 = centerX - tempA;
    qreal y1 = centerY - tempB;
    list.append(QPointF(x1,y1));
    // △ 左下的顶点
    qreal x2 = x1 - tempA;
    qreal y2 = y1 - tempB;
    list.append(QPointF(x2,y2));
    // △ 上部顶点，也就是箭头终点
    list.append(end);
    // △ 右下顶点
    tempA = width/2*std::sin(v);
    tempB = width/2*std::cos(v);
    qreal x3 = centerX + tempA;
    qreal y3 = centerY + tempB;
    list.append(QPointF(x3,y3));
    // △ 右下的顶点与底边中点之间中间位置的点
    qreal x4 = centerX + tempA/2;
    qreal y4 = centerY + tempB/2;
    list.append(QPointF(x4,y4));
    //箭头起点
    list.append(start);
    QVariant result = QVariant(list);
    return result;
}
void Cutter::askForQuit()
{
    auto reply = QMessageBox::question(nullptr,"系统提示","确定要退出截图吗");
    if (reply == QMessageBox::Yes) {
        QApplication::quit();
    }
}
void Cutter::createCutAreaByWindowRect()
{
    auto position = QCursor::pos();
    auto shoter = ScreenShoter::Get();
    for (int i = 0; i < shoter->desktopWindowRects.count(); ++i) {
        if(shoter->desktopWindowRects[i].contains(position)){
            setCutAreaLeft(shoter->desktopWindowRects[i].left());
            setCutAreaTop(shoter->desktopWindowRects[i].top());
            setCutAreaRight(shoter->desktopWindowRects[i].right());
            setCutAreaBottom(shoter->desktopWindowRects[i].bottom());
//            qDebug() << "position" << shoter->desktopWindowRects[i].left()
//                     << shoter->desktopWindowRects[i].top()
//                     << shoter->desktopWindowRects[i].right()
//                     << shoter->desktopWindowRects[i].bottom();
            return;
        }
    }
}
void Cutter::setCutAreaLeft(int val)
{
    if(val == cutAreaLeft) return;
    cutAreaLeft = val;
    emit cutAreaLeftChanged(val);
}
void Cutter::setCutAreaTop(int val)
{
    if(val == cutAreaTop) return;
    cutAreaTop = val;
    emit cutAreaTopChanged(val);
}
void Cutter::setCutAreaRight(int val)
{
    if(val == cutAreaRight) return;
    cutAreaRight = val;
    emit cutAreaRightChanged(val);
}
void Cutter::setCutAreaBottom(int val)
{
    if(val == cutAreaBottom) return;
    cutAreaBottom = val;
    emit cutAreaBottomChanged(val);
}


void Cutter::setMouseX(int val)
{
    if(val == mouseX) return;
    mouseX = val;
    emit mouseXChanged(val);
}
void Cutter::setMouseY(int val)
{
    if(val == mouseY) return;
    mouseY = val;
    emit mouseYChanged(val);
}
void Cutter::setColorRgb(QString val)
{
    if(val == colorRgb) return;
    colorRgb = val;
    emit colorRgbChanged(val);
}
void Cutter::setColorHex(QString val)
{
    if(val == colorHex) return;
    colorHex = val;
    emit colorHexChanged(val);
}
