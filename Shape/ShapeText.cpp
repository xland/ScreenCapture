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
void ShapeText::mouseMove(QMouseEvent* event)
{
    auto pos = event->position().toPoint();
    auto win = (WinBox*)parent();
    hoverDraggerIndex = -1;
    if (container->isVisible()) {
        if (!container->shapeTextInput->hasFocus()) {
            paintOnBoard();
            container->hide();
        }
    }
    else {
        if (container->ctrlRect.contains(pos)) {
            state = ShapeState::temp;
            win->winBoard->refresh();
            win->winCanvas->clear();
            win->winCanvas->curShape = this;
            container->show();
            container->raise();
            event->accept();
        }
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
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
