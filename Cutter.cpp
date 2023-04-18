
#include "Cutter.h"
#include <QCursor>
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>


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
    list.append(start);

    QLineF tarLine (start,end);
    qreal v = tarLine.angle();
    qreal centerX = end.x() - height * std::cos(v);
    qreal centerY = end.y() - height * std::sin(v);
    qreal tempA = width/4*std::cos(90 - v);
    qreal tempB = width/4*std::sin(90 - v);
    qreal x = centerX - tempA;
    qreal y = centerY - tempB;
    list.append(QPointF(x,y));
    x = x - tempA;
    y = y - tempB;
    list.append(QPointF(x,y));

    list.append(end);

    tempA = width/4*std::sin(v);
    tempB = width/4*std::cos(v);
    x = centerX + tempA*2;
    y = centerY + tempB*2;
    list.append(QPointF(x,y));


    x = centerX + tempA;
    y = centerY + tempB;
    list.append(QPointF(x,y));

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
