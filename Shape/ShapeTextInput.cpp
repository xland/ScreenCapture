#include <QPainter>
#include "ShapeTextInput.h"

ShapeTextInput::ShapeTextInput(QWidget* parent) : QTextEdit(parent)
{
	//setAttribute(Qt::WA_OpaquePaintEvent);
	//setAttribute(Qt::WA_NoSystemBackground);
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
	//QPainter painter(this);
	//painter.setRenderHint(QPainter::Antialiasing); 
	//painter.fillRect(rect(), Qt::transparent);
	//painter.setPen(QColor(128, 128, 128)); // »ÒÉ«±ß¿ò
	//painter.drawRect(rect().adjusted(1, 1, -1, -1)); 
	//textEdit->render(&painter);
	//QTextEdit::paintEvent(event);
}
