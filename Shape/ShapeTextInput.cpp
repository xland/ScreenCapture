#include <QPainter>
#include "ShapeTextInput.h"

ShapeTextInput::ShapeTextInput(QWidget* parent) : QTextEdit(parent)
{
	setFrameStyle(QFrame::NoFrame);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
	setAttribute(Qt::WA_QuitOnClose, false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setLineWrapMode(QTextEdit::NoWrap);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

ShapeTextInput::~ShapeTextInput()
{}

void ShapeTextInput::focusOutEvent(QFocusEvent * event)
{
	QTextEdit::focusOutEvent(event);
	emit focusOut();
}

void ShapeTextInput::focusInEvent(QFocusEvent* event)
{
	QTextEdit::focusInEvent(event);
	emit focusIn();
}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
	if (isVisible()) {
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(Qt::NoBrush);
		QPen pen;
		pen.setColor(textInputCursorColor);
		pen.setWidth(1);
		painter.setPen(pen);
		if (showTextInputCursor)
		{
			auto span = document()->toPlainText() == "" ? -1 : -4;
			auto cr = cursorRect().adjusted(0,1,0,span);
			painter.drawLine(cr.topLeft(),cr.bottomLeft());
		}
		pen.setStyle(Qt::CustomDashLine);
		pen.setDashPattern({ 3,3 });
		painter.setPen(pen);
		painter.drawRect(1, 1, viewport()->width()-2, viewport()->height() - 2);
	}
	showTextInputCursor = !showTextInputCursor;
}
