#include <QPainter>
#include <Windows.h>
#include <QTimer>
#include <QWindow>
#include "Win/WinBase.h"
#include "Win/Canvas.h"
#include <QDateTime>
#include <App/Util.h>

#include "ShapeTextInput.h"
#include "ShapeText.h"

ShapeTextInput::ShapeTextInput(ShapeText* shapeText, QWidget* parent) : QTextEdit(parent),shapeText{shapeText}
{
	setAttribute(Qt::WA_TranslucentBackground, true);
	setFrameStyle(QFrame::NoFrame);
	setLineWrapMode(QTextEdit::NoWrap);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); 
	setFocusPolicy(Qt::StrongFocus);

	auto f = Util::getTextFont(shapeText->fontSize);
	f->setWeight(shapeText->bold ? QFont::Bold : QFont::Normal);
	f->setItalic(shapeText->italic);
	setFont(*f);
	QString style{"color:%1;margin:0px;padding:0px;background:transparent;"};
	style = style.arg(shapeText->color.name());
	setStyleSheet(style);
}

ShapeTextInput::~ShapeTextInput()
{}

void ShapeTextInput::focusOutEvent(QFocusEvent* event)
{
	auto p = (ShapeTextContainer*)parent();
	auto rrr = p->geometry();
	auto ppp = QCursor::pos();
	if (rrr.contains(ppp)) return;
	QTextEdit::focusOutEvent(event);
	auto cursor = textCursor();
	cursor.clearSelection();
	setTextCursor(cursor);
	auto win = (WinBase*)shapeText->parent();
	auto text = document()->toPlainText();
	if (text.isEmpty())
	{
		parent()->deleteLater();
		win->canvas->removeShapeCur();
	}
	else {
		shapeText->state = ShapeState::ready;
		shapeText->containerRect = parentWidget()->geometry();
		shapeText->text = text;
		win->canvas->paintShapeOnBoard(shapeText);
		shapeText->container->deleteLater();
		shapeText->container = nullptr;
		win->canvas->shapeCur = nullptr;
	}
}

void ShapeTextInput::showEvent(QShowEvent* event)
{
	QTextEdit::showEvent(event);
	setFocus();
}
