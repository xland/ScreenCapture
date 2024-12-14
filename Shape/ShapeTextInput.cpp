#include <QPainter>
#include "ShapeTextInput.h"
#include "ShapeText.h"

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

ShapeTextInput* ShapeTextInput::create(ShapeText* parent)
{
	auto textEdit = new ShapeTextInput(); //不能把QObject设置为QWidget的parent
	auto font = textEdit->font();
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPointSize(parent->fontSize);
	font.setWeight(parent->bold ? QFont::Bold : QFont::Normal);
	font.setItalic(parent->italic);
	textEdit->setFont(font);
	QString style{ "color:%1;background:transparent;margin:1px;padding:2px;caret-color:%1;"};
	style = style.arg(parent->color.name());
	textEdit->textInputCursorColor = parent->color;
	textEdit->setStyleSheet(style);
	connect(textEdit->document(), &QTextDocument::contentsChanged, textEdit, &ShapeTextInput::adjustSize);
	connect(textEdit, &ShapeTextInput::focusOut, parent, &ShapeText::focusOut);
	connect(textEdit, &ShapeTextInput::focusIn, parent, &ShapeText::focusIn);
	return textEdit;
}

void ShapeTextInput::moveTo(const QPoint& pos)
{
	move(pos);
	show();
	setFocus();
	setText("");
	raise();
}

void ShapeTextInput::adjustSize()
{
	auto doc = document();
	doc->adjustSize();
	auto size = doc->size().toSize();
	if (doc->isEmpty()) {
		size += QSize(8, 6);
	}
	else {
		size += QSize(6, 2);
	}
	setFixedSize(size);
}

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
