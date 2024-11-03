#include "ShapeTextInput.h"

ShapeTextInput::ShapeTextInput(QWidget* parent) : QTextEdit(parent)
{}

ShapeTextInput::~ShapeTextInput()
{}

void ShapeTextInput::focusOutEvent(QFocusEvent * event)
{
	QTextEdit::focusOutEvent(event);
	emit focusOut();
}
