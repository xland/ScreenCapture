#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>
#include <QTextOption>
#include <QWindow>

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
    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");
}

ShapeText::~ShapeText()
{
    /*delete textEdit;*/
}

void ShapeText::focusOut()
{
    auto text = textEdit->document()->toPlainText().trimmed();
    if (text.isEmpty()) {
        deleteLater();
        return;
    }
    paintOnBoard();
}

void ShapeText::focusIn()
{
    //auto win = (WinBox*)parent();
    //win->winCanvas->clear();
}

void ShapeText::paint(QPainter* painter)
{
    if (textEdit->isVisible()) return;
    RECT rectNative;
    auto hwnd = (HWND)textEdit->winId();
    GetWindowRect(hwnd, &rectNative);
    auto img = textEdit->grab().toImage();
    QRectF rect(QPointF(rectNative.left, rectNative.top), QPointF(rectNative.right, rectNative.bottom));
    painter->drawImage(rect, img);
}
void ShapeText::paintDragger(QPainter* painter)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(Qt::CustomDashLine);
    pen.setDashPattern({ 3, 3 });
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    auto win = (WinBox*)parent();
    auto rect = textEdit->getNativeRect();
    rect.adjust(2, -2, -4, -4);
    painter->drawRect(rect);
}

void ShapeText::mouseMove(QMouseEvent* event)
{
    auto pos = event->position().toPoint();
    auto win = (WinBox*)parent();
    auto rect = textEdit->getNativeRect();
    auto innerRect = rect.adjusted(3, 3, -3, -3);
    hoverDraggerIndex = -1;
    if (rect.contains(pos)) {
        if (innerRect.contains(pos)) {
            hoverDraggerIndex = 0;
            QGuiApplication::setOverrideCursor(Qt::IBeamCursor);
        }
        else {
            hoverDraggerIndex = 8;
            QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
        }
        if (!textEdit->isVisible()) {
            textEdit->show();
            textEdit->setFocus();
            textEdit->raise();
        }        
        event->accept();
    }
    else {
        if (textEdit->isVisible()&& !textEdit->hasFocus()) {
            textEdit->hide();
        }
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
    if (!textEdit) {
        pressPos = event->position();
        textEdit = ShapeTextInput::create(this);
        textEdit->moveTo(QCursor::pos() + QPoint(-10, -10));
        event->accept();
        return;
    }
    if (hoverDraggerIndex == 0) {
        state = ShapeState::sizing0;
        event->accept();
    }else if(hoverDraggerIndex == 8){
        pressPos = QCursor::pos();
		state = ShapeState::moving;
        textEdit->setFocus();
		event->accept();
	}
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    //if (hoverDraggerIndex == 0) {
    //    auto cursorPos = textEdit->mapFromGlobal(QCursor::pos());
    //    auto cursor = textEdit->cursorForPosition(cursorPos);
    //    textEdit->setTextCursor(cursor);
    //    //textEdit->setFocus();
    //    event->accept();
    //}
}
void ShapeText::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        auto p = textEdit->pos() + span;
		textEdit->move(p.x(), p.y());
        pressPos = pos;
        event->accept();
    }
}
