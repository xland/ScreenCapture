#include <qpainter.h>
#include <QPen>

#include "ShapeEllipse.h"
#include "../App/App.h"
#include "../Win/WinBox.h"
#include "../Tool/ToolSub.h"

ShapeEllipse::ShapeEllipse(QObject* parent) : ShapeRectBase(parent)
{
    auto win = (WinBox*)parent;
    isFill = win->toolSub->getSelectState("ellipseFill");
    color = win->toolSub->getColor();
    strokeWidth = win->toolSub->getStrokeWidth();
}

ShapeEllipse::~ShapeEllipse()
{
}

void ShapeEllipse::paint(QPainter* painter)
{
    if (isFill) {
        painter->setBrush(QBrush(color));
        painter->setPen(Qt::NoPen);
    }
    else {
        QPen pen(color);
        pen.setWidthF(strokeWidth);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
    }
	painter->drawEllipse(shape);
}
void ShapeEllipse::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->position();
    auto center = shape.center();
    auto win = (WinBox*)parent();
    if (isFill) {
        double normalizedX = (pos.x() - center.x()) / static_cast<double>(shape.width() / 2);
        double normalizedY = (pos.y() - center.y()) / static_cast<double>(shape.height() / 2);
        auto flag = (normalizedX * normalizedX + normalizedY * normalizedY <= 1.0);
        if (flag) {
            hoverDraggerIndex = 8;
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
    else {
        float half{ strokeWidth / 2.f };
        QRectF outerRect = shape.adjusted(-half, -half, half, half);
        qreal spanX{ pos.x() - center.x() }, spanY{ pos.y() - center.y() };
        float normalizedX = spanX / static_cast<double>(outerRect.width() / 2);
        float normalizedY = spanY / static_cast<double>(outerRect.height() / 2);
        auto flag = (normalizedX * normalizedX + normalizedY * normalizedY <= 1.0);
        if (flag) {
            QRectF innerRect = shape.adjusted(half, half, -half, -half);
            normalizedX = spanX / static_cast<double>(innerRect.width() / 2);
            normalizedY = spanY / static_cast<double>(innerRect.height() / 2);
            flag = (normalizedX * normalizedX + normalizedY * normalizedY <= 1.0);
            if (!flag) {
                hoverDraggerIndex = 8;
                win->updateCursor(Qt::SizeAllCursor);
            }
        }
    }
}
