#include <QPainter>
#include <Windows.h>
#include <QVBoxLayout>
#include <QWindow>

#include "ShapeTextInput.h"
#include "../Win/WinBase.h"
#include "ShapeTextContainer.h"
#include "ShapeText.h"

ShapeTextContainer::ShapeTextContainer(ShapeText* shapeText, QWidget* parent) : QWidget(parent),shapeText{shapeText}
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setAttribute(Qt::WA_InputMethodEnabled, true);
	setAttribute(Qt::WA_QuitOnClose, false);
	setFocusPolicy(Qt::StrongFocus);
	setCursor(Qt::SizeAllCursor);

	shapeTextInput = new ShapeTextInput(shapeText,this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(3,3,3,3);
	layout->setSpacing(0);
	layout->addWidget(shapeTextInput);
	setLayout(layout);

	connect(shapeTextInput->document(), &QTextDocument::contentsChanged, this, &ShapeTextContainer::adjustSize);

}

ShapeTextContainer::~ShapeTextContainer()
{
}

void ShapeTextContainer::adjustSize()
{
	auto doc = shapeTextInput->document();
	doc->adjustSize();
	setFixedSize(doc->size().toSize()+QSize(6,6));
}

void ShapeTextContainer::paintEvent(QPaintEvent* event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(QColor(0, 0, 0,1));
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect());
	QPen pen;
	pen.setColor(shapeText->color);
	pen.setStyle(Qt::CustomDashLine);
	pen.setDashPattern({ 3,3 });
	pen.setWidth(1);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	QRectF rect(1, 1, width() - 2, height() - 2);
	painter.drawRect(rect);
}

void ShapeTextContainer::mouseMoveEvent(QMouseEvent* event)
{
	auto win = (WinBase*)window();
	if (event->buttons() & Qt::LeftButton) {
		auto pos = QCursor::pos();
		auto span = pos - pressPos;
		auto p = mapToGlobal(QPoint(0, 0)) + span;
		move(p.x(), p.y());
		pressPos = pos;
	}
}

void ShapeTextContainer::mousePressEvent(QMouseEvent* event)
{
	pressPos = QCursor::pos();
}

void ShapeTextContainer::mouseReleaseEvent(QMouseEvent* event)
{
	shapeTextInput->setFocus();
}

void ShapeTextContainer::showEvent(QShowEvent* event)
{
	raise();
	activateWindow();
	shapeTextInput->setFocus();
}
