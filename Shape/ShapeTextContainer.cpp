#include <QPainter>
#include <Windows.h>
#include <QVBoxLayout>
#include <QWindow>

#include "ShapeTextInput.h"
#include "../Win/WinBox.h"
#include "ShapeTextContainer.h"
#include "ShapeText.h"

ShapeTextContainer::ShapeTextContainer(ShapeText* shapeText, QWidget* parent) : QWidget(parent),shapeText{shapeText}
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TransparentForMouseEvents, false);

	shapeTextInput = new ShapeTextInput(shapeText,this);
	QVBoxLayout* layout = new QVBoxLayout(this);
	layout->setContentsMargins(3, 3, 3,3);
	layout->setSpacing(0);
	layout->addWidget(shapeTextInput);
	setLayout(layout);

	connect(shapeTextInput->document(), &QTextDocument::contentsChanged, this, &ShapeTextContainer::adjustSize);
	setCursor(Qt::SizeAllCursor);
	move(QCursor::pos() + QPoint(-10, -10));
	show();
	raise();
}

ShapeTextContainer::~ShapeTextContainer()
{
}

QRect ShapeTextContainer::getNativeRect()
{
	RECT rect;
	GetWindowRect((HWND)winId(), &rect);
	POINT lt{ .x{rect.left},.y{rect.top} };
	POINT rb{ .x{rect.right},.y{rect.bottom} };
	auto win = (WinBox*)shapeText->parent();
	ScreenToClient(win->hwnd,&lt);
	ScreenToClient(win->hwnd,&rb);
	return QRect(QPoint(lt.x, lt.y+8), QPoint(rb.x, rb.y));
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
	if (painting) return;
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.setBrush(QColor(0, 0, 0,6));
	painter.setPen(Qt::NoPen);
	painter.drawRect(rect());
	QPen pen;
	pen.setColor(shapeText->color);
	pen.setStyle(Qt::CustomDashLine);
	pen.setDashPattern({ 3,3 });
	pen.setWidth(1);
	painter.setPen(pen);
	painter.setBrush(Qt::NoBrush);
	QRectF rect(2, 2, width() - 4, height() - 4);
	painter.drawRect(rect);
}
