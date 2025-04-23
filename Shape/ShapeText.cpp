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
#include "../Win/WinPin.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/Canvas.h"
#include "App/Util.h"


ShapeText::ShapeText(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");
}

ShapeText::~ShapeText()
{
}

void ShapeText::paint(QPainter* painter)
{
    if (state != ShapeState::ready) return;
    auto f = Util::getTextFont(fontSize);
    f->setWeight(bold ? QFont::Bold : QFont::Normal);
    f->setItalic(italic);
	painter->setFont(*f);
    painter->setPen(color);
	painter->drawText(containerRect.adjusted(7,-1,0,0), Qt::AlignLeft | Qt::AlignVCenter, text);
}
bool ShapeText::mouseMove(QMouseEvent* event)
{
    auto ppp = event->pos();
    auto win = (WinBase*)parent();
    if (containerRect.contains(ppp)) {
        win->setCursor(Qt::IBeamCursor);
        hoverDraggerIndex = 8;
        return true;
    }
    else {
		hoverDraggerIndex = -1;
        if (win->cursor() == Qt::IBeamCursor && win->state != State::text) {
            win->setCursor(Qt::CrossCursor);
        }
        return false;
    }
}
bool ShapeText::mousePress(QMouseEvent* event)
{
    auto win = (WinBase*)parent();
    auto ppp = event->pos();
    if (containerRect.contains(ppp)) {
        state = ShapeState::hidden;
        win->canvas->shapeCur = nullptr;
        win->canvas->removeShapeFromBoard(this);
        container = new ShapeTextContainer(this,win);
        container->move(containerRect.topLeft());
        container->show();
        container->shapeTextInput->setText(text);
        win->canvas->shapeCur = nullptr;
        return true;
    }
    if(state == ShapeState::temp) {
        container = new ShapeTextContainer(this,win);
        container->move(QCursor::pos() + QPoint(-10, -10));
        container->show();
        container->shapeTextInput->setText("");  //触发一次adjustSize
        return true;
    }
    return false;
}
bool ShapeText::mouseRelease(QMouseEvent* event)
{
    return true;
}
