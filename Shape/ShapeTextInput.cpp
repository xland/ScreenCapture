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
	if (showTextInputCursor && isVisible()) {
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::LosslessImageRendering, true);
		painter.setBrush(textInputCursorColor);
		painter.setPen(Qt::NoPen);
		painter.drawRect(cursorRect());
	}
	showTextInputCursor = !showTextInputCursor;
}
