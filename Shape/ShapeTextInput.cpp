#include <QPainter>
#include "ShapeTextInput.h"

ShapeTextInput::ShapeTextInput(QWidget* parent) : QTextEdit(parent)
{
	setFrameStyle(QFrame::NoFrame);
	//setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_NoSystemBackground,true);
	//setAttribute(Qt::WA_TranslucentBackground);
}

ShapeTextInput::~ShapeTextInput()
{}

void ShapeTextInput::focusOutEvent(QFocusEvent * event)
{
	QTextEdit::focusOutEvent(event);
	emit focusOut();
}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	QTextEdit::paintEvent(event);
	painter.setRenderHint(QPainter::Antialiasing); 
	painter.fillRect(rect(), Qt::transparent);
	painter.setPen(QColor(128, 128, 128)); // 灰色边框
	painter.drawRect(rect().adjusted(1, 1, -1, -1)); 
}
