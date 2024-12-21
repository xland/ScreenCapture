#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>
#include <QTextOption>
#include <QTimer>

#include "ShapeText.h"
#include "ShapeTextInput.h"
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
    RECT rectNative;
    auto hwnd = (HWND)container->winId();
    GetWindowRect(hwnd, &rectNative);
    auto img = container->grab().toImage();
    QRectF rect(QPointF(rectNative.left, rectNative.top), QPointF(rectNative.right, rectNative.bottom));
    painter->drawImage(rect, img);

    QPen pen;
    pen.setColor(color);
    pen.setStyle(Qt::CustomDashLine);
    pen.setDashPattern({ 3, 3 });
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(rect);
}
void ShapeText::mouseMove(QMouseEvent* event)
{
    auto pos = event->position().toPoint();
    auto win = (WinBox*)parent();
    auto inner = container->getNativeRect();
    auto rect = inner.adjusted(-2,-2,2,2);
    hoverDraggerIndex = -1;
    if (!container->isVisible()) {
        if (rect.contains(pos)) {
            if (inner.contains(pos)) {
                hoverDraggerIndex = 0;
            }
            else {
                hoverDraggerIndex = 8;
                QGuiApplication::setOverrideCursor(Qt::SizeAllCursor);
            }
            state = ShapeState::temp;
			win->winBoard->refresh();
            container->show();
            container->raise();
            //container->shapeTextInput->setFocus(Qt::FocusReason::MouseFocusReason);
            QTimer::singleShot(100, container->shapeTextInput, [this]() {
                container->shapeTextInput->setFocus(Qt::FocusReason::MouseFocusReason);
                });
            event->accept();
        }
    }
    else {
        if (container->isVisible() && !container->shapeTextInput->hasFocus()) {
            paintOnBoard();
            container->hide();
        }
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
    if (container && container->isVisible()) {
        paintOnBoard();
        container->hide();
    }
    if (!container) {
        container = new ShapeTextContainer(this);
        state = ShapeState::sizing0;
        event->accept();
        return;
    }
    if(hoverDraggerIndex == 8){
		state = ShapeState::moving;
		event->accept();
	}
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    state = ShapeState::ready;
}
void ShapeText::mouseDrag(QMouseEvent* event)
{

}
