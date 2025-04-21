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
    delete container;
}

void ShapeText::paint(QPainter* painter)
{
    if (state != ShapeState::ready) return;
    auto img = container->shapeTextInput->grab().toImage();
    auto win = (WinBase*)parent();
    img.setDevicePixelRatio(win->devicePixelRatio());
    painter->drawImage(container->pos()+container->shapeTextInput->pos(), img);
}
bool ShapeText::mouseMove(QMouseEvent* event)
{
    return false;
}
bool ShapeText::mousePress(QMouseEvent* event)
{
    if (!container) {
        container = new ShapeTextContainer(this);
        state = ShapeState::sizing0;
        return true;
    }
    if(hoverDraggerIndex == 8){
		state = ShapeState::moving;
        return true;
	}
    return false;
}
bool ShapeText::mouseRelease(QMouseEvent* event)
{
    return true;
}
