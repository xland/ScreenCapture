#include "CutMask.h"
#include "CanvasWidget.h"

namespace {
	std::unique_ptr<CutMask> cutMask;
}

CutMask::CutMask(QWidget *parent) : QWidget(parent)
{
	setGeometry(parent->rect());
	maskRect.setRect(-maskStroke, -maskStroke, width() + maskStroke, height() + maskStroke);
	auto size = geometry();
	show();
}

CutMask::~CutMask()
{}

void CutMask::Init()
{
	auto parent = CanvasWidget::Get();
	cutMask = std::make_unique<CutMask>(parent);
	//auto winNative = WindowNative::Get();
	//maskRect.setRect(-maskStroke, -maskStroke, winNative->w + maskStroke, winNative->h + maskStroke);
}

CutMask* CutMask::Get()
{
	return cutMask.get();
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
	QWidget::paintEvent(event);
}

void CutMask::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		dragging = true;
		dragPosition = event->pos();
		if (CanvasWidget::Get()->state == State::start) {
			maskRect.setRect(dragPosition.x(), dragPosition.y(), 0, 0);
			CanvasWidget::Get()->state = State::mask;
		}
	}
	else if (event->button() == Qt::RightButton) {
		qApp->quit();
	}
	event->accept();
}

void CutMask::mouseMoveEvent(QMouseEvent* event)
{
	if (dragging && event->buttons() & Qt::LeftButton) {

		if (CanvasWidget::Get()->state == State::mask) {
			maskRect.setBottomRight(event->pos());
			update();
		}
	}
	event->accept();
}

void CutMask::mouseReleaseEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton) {
		if (CanvasWidget::Get()->state == State::mask) {
			CanvasWidget::Get()->state = State::tool;
			//auto btn = new QPushButton("allen", this);
			//btn->move(pos);
			//btn->show();

			ToolMain::Show();

		}
		dragging = false;
	}
	event->accept();
}
