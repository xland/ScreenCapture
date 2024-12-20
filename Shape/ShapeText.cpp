#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>
#include <QTextOption>

#include "ShapeText.h"
#include "../Win/WinBase.h"
#include "../Win/WinBox.h"
#include "../Win/WinBoard.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinCanvas.h"


ShapeText::ShapeText(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBox*)parent;
    //要自己控制删除策略，不然第二个实例创建时，第一个实例的blur还没有触发，会导致第一个实例被删除
    //state = ShapeState::ready;
    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");
}

ShapeText::~ShapeText()
{
    delete container;
}

void ShapeText::focusOut()
{
    //auto text = textEdit->document()->toPlainText();
    //if (text.isEmpty()) {
    //    state = ShapeState::temp;
    //    deleteLater();
    //    return;
    //}    
    //paintOnBoard();
    //textEdit->hide();
}

void ShapeText::focusIn()
{
    //state = ShapeState::temp;
    //auto win = (WinBox*)parent();
    //win->winBoard->refresh();
}

void ShapeText::paint(QPainter* painter)
{
    container->painting = true;
    RECT rectNative;
    auto hwnd = (HWND)container->winId();
    GetWindowRect(hwnd, &rectNative);
    auto img = container->grab().toImage();
    QRectF rect(QPointF(rectNative.left, rectNative.top), QPointF(rectNative.right, rectNative.bottom));
    painter->drawImage(rect, img);
    container->painting = false;
}
void ShapeText::paintDragger(QPainter* painter)
{
    //QPen pen;
    //pen.setColor(color);
    //pen.setStyle(Qt::CustomDashLine);
    //pen.setDashPattern({ 3, 3 });
    //pen.setWidth(1);
    //painter->setPen(pen);
    //painter->setBrush(Qt::NoBrush);
    //auto win = (WinBox*)parent();
    //auto rect = textEdit->getNativeRect();
    //rect.adjust(2, -2, -4, -4);
    //painter->drawRect(rect);
}
void ShapeText::mouseMove(QMouseEvent* event)
{
    //auto pos = event->position().toPoint();
    //auto win = (WinBox*)parent();
    //auto rect = textEdit->getNativeRect();
    //auto outerRect = rect.adjusted(-3, -3, 3, 3);
    //hoverDraggerIndex = -1;
    //if (outerRect.contains(pos)) {
    //    if (!rect.contains(pos)) {
    //        hoverDraggerIndex = 8;
    //        QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
    //    }
    //    else if (!textEdit->isVisible()) {
    //        textEdit->show();
    //        textEdit->setFocus();
    //        textEdit->raise();
    //    }        
    //    event->accept();
    //}
    //else {
    //    if (textEdit->isVisible()&& !textEdit->hasFocus()) {
    //        textEdit->hide();
    //    }
    //}
}
void ShapeText::mousePress(QMouseEvent* event)
{
    qDebug() << "mousePress";
    if (!container) {
        pressPos = event->position();
        container = new ShapeTextContainer(this);
        state = ShapeState::sizing0;
        event->accept();
        return;
    }
    if(hoverDraggerIndex == 8){
        pressPos = QCursor::pos();
		state = ShapeState::moving;
		event->accept();
	}
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    qDebug() << "mouseRelease";
    if (state == ShapeState::sizing0) {
        //auto cursorPos = textEdit->mapFromGlobal(QCursor::pos());
        //auto cursor = textEdit->cursorForPosition(cursorPos);
        //textEdit->setTextCursor(cursor);
        //textEdit->setFocus();
        state = ShapeState::ready;
        event->accept();
    }
}
void ShapeText::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::moving) {
  //      auto pos = event->pos();
  //      auto span = pos - pressPos;
  //      auto p = textEdit->pos() + span;
		//textEdit->move(p.x(), p.y());
  //      pressPos = pos;
        event->accept();
    }
}
