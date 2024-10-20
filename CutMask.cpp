#include <qcursor.h>

#include "CutMask.h"
#include "CanvasWidget.h"
#include "Tool/ToolMain.h"
#include "WindowNative.h"


CutMask::CutMask(QWidget *parent) : QWidget(parent)
{
	setMouseTracking(true);
	setGeometry(parent->rect());
	maskRect.setRect(-maskStroke, -maskStroke, width() + maskStroke, height() + maskStroke);
	show();
}

CutMask::~CutMask()
{
}

void CutMask::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true); 
	QPainterPath temp0;
	temp0.addRect(-maskStroke, -maskStroke, width() + maskStroke, height() + maskStroke);
	QPainterPath temp1;
	temp1.addRect(maskRect);
	auto mask = temp0.subtracted(temp1);

	painter.setPen(QPen(QBrush(QColor(22, 119, 255)), maskStroke));
	painter.setBrush(QBrush(QColor(0, 0, 0, 120)));
	painter.drawPath(mask);
}

void CutMask::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		dragPosition = event->pos();
		auto canvasWidget = CanvasWidget::Get();
		if (canvasWidget->state == State::start) {
			canvasWidget->state = State::mask;
			maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
		}
		else if (canvasWidget->state == State::tool) {
			canvasWidget->toolMain->hide();
			if (mousePosState != 0) {
				changeMaskRect(dragPosition);
			}
		}
		else {
			//QCoreApplication::sendEvent(parentWidget(), event);
			
		}
	}
	else if (event->button() == Qt::RightButton) {
		parentWidget()->close();
		WindowNative::Close();
	}
	event->ignore();
}

void CutMask::mouseMoveEvent(QMouseEvent* event)
{
	auto canvasWidget = CanvasWidget::Get();
	auto pos = event->pos();
	if (dragging) {
		if (event->buttons() & Qt::LeftButton) {
			if (canvasWidget->state == State::mask) {
				maskRect.setBottomRight(pos);
				update();
			}
			else if (canvasWidget->state == State::tool) {
				if (mousePosState == 0) {
					auto span = pos - dragPosition;
					maskRect.moveTo(maskRect.topLeft() + span);
					dragPosition = pos;
					update();
				}
				else{
					changeMaskRect(pos);
				}
			}
			else {
				//QCoreApplication::sendEvent(parentWidget(), event);
			}
		}
	}
	else if (canvasWidget->state == State::tool) {
		changeMousePosState(pos);
	}
	else {
		setCursor(Qt::CrossCursor);
	}
	event->ignore();
}

void CutMask::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		maskRect = maskRect.normalized();
		auto canvasWidget = CanvasWidget::Get();
		if (canvasWidget->state == State::mask) {
			canvasWidget->state = State::tool;
			canvasWidget->toolMain->show();

		}
		else if (canvasWidget->state == State::tool) {
			canvasWidget->toolMain->show();
		}
		else {
			//QCoreApplication::sendEvent(parentWidget(), event);
		}
		dragging = false;
	}
	event->ignore();
}

void CutMask::changeMaskRect(const QPoint& pos)
{
	if (mousePosState == 1) {
		maskRect.setTopLeft(pos);
	}
	else if (mousePosState == 2) {
		maskRect.setTop(pos.y());
	}
	else if (mousePosState == 3) {
		maskRect.setTopRight(pos);
	}
	else if (mousePosState == 4) {
		maskRect.setRight(pos.x());
	}
	else if (mousePosState == 5) {
		maskRect.setBottomRight(pos);
	}
	else if (mousePosState == 6) {
		maskRect.setBottom(pos.y());
	}
	else if (mousePosState == 7) {
		maskRect.setBottomLeft(pos);
	}
	else if (mousePosState == 8) {
		maskRect.setLeft(pos.x());
	}
	update();
}

void CutMask::changeMousePosState(const QPoint& pos)
{
	auto x = pos.x(); auto y = pos.y();
	auto leftX = maskRect.topLeft().x(); auto topY = maskRect.topLeft().y();
	auto rightX = maskRect.bottomRight().x(); auto bottomY = maskRect.bottomRight().y();
	if (maskRect.contains(pos)) {
		setCursor(Qt::SizeAllCursor);
		mousePosState = 0;
	}
	else if (x < leftX && y < topY) {
		setCursor(Qt::SizeFDiagCursor);
		mousePosState = 1;
	}
	else if (x >= leftX && x < rightX && y < topY) {
		setCursor(Qt::SizeVerCursor);
		mousePosState = 2;
	}
	else if (x >= rightX && y < topY) {
		setCursor(Qt::SizeBDiagCursor);
		mousePosState = 3;
	}
	else if (x >= rightX && y >= topY && y < bottomY) {
		setCursor(Qt::SizeHorCursor);
		mousePosState = 4;
	}
	else if (x >= rightX && y >= bottomY) {
		setCursor(Qt::SizeFDiagCursor);
		mousePosState = 5;
	}
	else if (x >= leftX && x < rightX && y >= bottomY) {
		setCursor(Qt::SizeVerCursor);
		mousePosState = 6;
	}
	else if (x < leftX && y >= bottomY) {
		setCursor(Qt::SizeBDiagCursor);
		mousePosState = 7;
	}
	else if (x < leftX && y < bottomY && y >= topY) {
		setCursor(Qt::SizeHorCursor);
		mousePosState = 8;
	}
}
