#include <QPainter>
#include <Windows.h>
#include "../Win/WinBox.h"

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
	textEdit->parent = parent;
	auto font = textEdit->font();
	font.setStyleStrategy(QFont::PreferAntialias);
	font.setPointSize(parent->fontSize);
	font.setWeight(parent->bold ? QFont::Bold : QFont::Normal);
	font.setItalic(parent->italic);
	textEdit->setFont(font);
	QString style{"color:%1;background:transparent;margin:0px;padding:2px;"};
	style = style.arg(parent->color.name());
	textEdit->textInputCursorColor = parent->color;
	textEdit->setStyleSheet(style);
	auto doc = textEdit->document();
	connect(doc, &QTextDocument::contentsChanged, textEdit, &ShapeTextInput::adjustSize);
	connect(textEdit, &ShapeTextInput::focusOut, parent, &ShapeText::focusOut);
	connect(textEdit, &ShapeTextInput::focusIn, parent, &ShapeText::focusIn);

	connect(qApp, &QGuiApplication::focusWindowChanged, [textEdit](QWindow* focusWindow) {
		if (textEdit->parent->state == ShapeState::temp) return;
		if (!textEdit->isVisible()) return;
		auto handle = textEdit->windowHandle();
		if (focusWindow != handle) {
			textEdit->hide();
			emit textEdit->focusOut();
		}
	});

	return textEdit;
}

void ShapeTextInput::moveTo(const QPoint& pos)
{
	move(pos);
	show();
	setText("123"); //触发一次adjustSize
	setFocus();
	raise();
}

QRect ShapeTextInput::getNativeRect()
{
	RECT rect;
	GetWindowRect((HWND)winId(), &rect);
	POINT lt{ .x{rect.left},.y{rect.top} };
	POINT rb{ .x{rect.right},.y{rect.bottom} };
	auto win = (WinBox*)parent->parent();
	ScreenToClient(win->hwnd,&lt);
	ScreenToClient(win->hwnd,&rb);
	return QRect(QPoint(lt.x, lt.y+8), QPoint(rb.x, rb.y));
}

void ShapeTextInput::adjustSize()
{
	auto doc = document();
	doc->adjustSize();
	auto size = doc->size().toSize();
	if (doc->isEmpty()) {
		size += QSize(8, 4);
	}
	else {
		size += QSize(6, 2);
	}
	setFixedSize(size);
}

//void ShapeTextInput::focusOutEvent(QFocusEvent * event)
//{
//	qDebug() << "allen";
//	qDebug() << "allen";
//	auto r = event->reason();
//	qDebug() << r;
//	auto tar = qApp->focusWindowChanged;
//	if (r == Qt::MouseFocusReason || r == Qt::ActiveWindowFocusReason) {
//		QTextCursor cursor = textCursor();
//		cursor.clearSelection();
//		setTextCursor(cursor);
//	}
//	else {
//		QTextEdit::focusOutEvent(event);
//		hide();
//		emit focusOut();
//	}
//}

//void ShapeTextInput::focusInEvent(QFocusEvent* event)
//{
//	QTextEdit::focusInEvent(event);
//	//emit focusIn();
//}

void ShapeTextInput::paintEvent(QPaintEvent* event)
{
	QTextEdit::paintEvent(event);
	if (isVisible()) {
		QPainter painter(viewport());
		painter.setRenderHint(QPainter::Antialiasing, true);
		painter.setBrush(Qt::NoBrush);
		QPen pen;
		pen.setColor(textInputCursorColor);
		pen.setWidthF(1);
		painter.setPen(pen);
		if (showTextInputCursor)
		{
			auto span = document()->toPlainText() == "" ? -1 : -2;
			auto cr = cursorRect().adjusted(0,2,0,span);
			painter.drawLine(cr.topLeft(),cr.bottomLeft());
		}
		pen.setStyle(Qt::CustomDashLine);
		pen.setDashPattern({ 3,3 });
		painter.setPen(pen);
		painter.drawRect(1, 1, viewport()->width()-2, viewport()->height()-2);
	}
	showTextInputCursor = !showTextInputCursor;
}
