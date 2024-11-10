#include <qpainter.h>

#include "ShapeEllipse.h"
#include "../App/App.h"
#include "../Win/WinBase.h"
#include "../Tool/ToolSub.h"

ShapeEllipse::ShapeEllipse(QObject* parent) : ShapeRect(parent, "ellipseFill")
{
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
        painter->setPen(QPen(QBrush(color), strokeWidth));
        painter->setBrush(Qt::NoBrush);
    }
	painter->drawEllipse(shape);
}
void ShapeEllipse::mouseOnShape(QMouseEvent* event)
{
    auto pos = event->pos();
    auto center = shape.center();
    if (isFill) {
        double normalizedX = (pos.x() - center.x()) / static_cast<double>(shape.width() / 2);
        double normalizedY = (pos.y() - center.y()) / static_cast<double>(shape.height() / 2);
        auto flag = (normalizedX * normalizedX + normalizedY * normalizedY <= 1.0);
        if (flag) {
            hoverDraggerIndex = 8;
            auto win = (WinBase*)parent();
            win->updateCursor(Qt::SizeAllCursor);
        }
    }
    else {
        float half{ strokeWidth / 2.f };
        QRectF outerRect = shape.adjusted(-half, -half, half, half);
        int spanX{ pos.x() - center.x() }, spanY{ pos.y() - center.y() };
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
                auto win = (WinBase*)parent();
                win->updateCursor(Qt::SizeAllCursor);
            }
        }
    }
}
