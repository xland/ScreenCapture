#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>
#include <QTextOption>

#include "ShapeText.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"


ShapeText::ShapeText(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    state = ShapeState::ready;
    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");
}

ShapeText::~ShapeText()
{
}

void ShapeText::adjustSize()
{
    textEdit->document()->adjustSize();
    auto size = textEdit->document()->size().toSize();
    if (textEdit->document()->isEmpty()) {
        size += QSize(12, 6); 
    }
    else {
        size += QSize(6, 2);
    }
    textEdit->setFixedSize(size);
    qDebug() << "text adjustSize";
    auto win = (WinBase*)parent();
    win->update();
}

void ShapeText::createTextEdit()
{
    textEdit = new ShapeTextInput((QWidget*)parent());
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用垂直滚动条
    textEdit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // 禁用水平滚动条
    //textEdit set topmost
	textEdit->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    QFont font = textEdit->font();
    font.setPointSize(fontSize);
    if (bold) {
        font.setWeight(QFont::Bold);
    }
    font.setItalic(italic);
    textEdit->setFont(font);
    QString style{ "color:%1;background:transparent;margin:1px;padding:2px;" };
    style = style.arg(color.name());
    textEdit->setStyleSheet(style);
    connect(textEdit->document(), &QTextDocument::contentsChanged, this, &ShapeText::adjustSize);
    connect(textEdit, &ShapeTextInput::focusOut, this, &ShapeText::focusOut);
}

void ShapeText::focusOut()
{
    auto text = textEdit->document()->toPlainText().trimmed();
    if (text.isEmpty()) {
        state = ShapeState::temp;
    }
    textEdit->hide();
    qDebug() << "text focus out";
}

void ShapeText::paint(QPainter* painter)
{
    if (textEdit->isVisible()) {
        paintBorder(painter);
    }
    else {
        this->textEdit->render(painter, textEdit->pos());
    }    
}
void ShapeText::paintDragger(QPainter* painter)
{
    if (textEdit->isVisible()) return;
    paintBorder(painter);
}
void ShapeText::paintBorder(QPainter* painter)
{
    QPen pen;
    pen.setColor(color);
    pen.setStyle(Qt::DashLine);
    pen.setDashOffset(1);
    pen.setDashPattern({ 1, 2 });
    pen.setWidth(1);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    auto rect = textEdit->geometry();
    painter->drawRect(rect);
}
void ShapeText::mouseMove(QMouseEvent* event)
{
    auto pos = event->pos();
    auto rect = textEdit->geometry();
    auto innerRect = rect.adjusted(2, 2, -2, -2);
    auto outerRect = rect.adjusted(-1, -1, 1, 1);
    hoverDraggerIndex = -1;
    if (outerRect.contains(pos)) {
        auto win = (WinBase*)parent();
        if (innerRect.contains(pos)) {
            hoverDraggerIndex = 0;
            win->updateCursor(Qt::IBeamCursor);
        }
        else {
            hoverDraggerIndex = 8;
            win->updateCursor(Qt::SizeAllCursor);
        }
        win->canvas->changeShape(this,true);
        event->accept();
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
    if (!textEdit) {
        createTextEdit();
        textEdit->move(event->pos() + QPoint(-10, -10));
        textEdit->show();
        textEdit->setFocus();
        textEdit->setText(""); //用于触发adjustSize
        return;
    }
    if (hoverDraggerIndex == 0) {
        textEdit->show();
        textEdit->setFocus();
        auto win = (WinBase*)parent();
        win->update();
        event->accept();
        auto cursorPos = textEdit->mapFromGlobal(event->pos());
		auto cursor = textEdit->cursorForPosition(cursorPos);
		textEdit->setTextCursor(cursor);
        return;
    }else if(hoverDraggerIndex == 8){
        textEdit->show();
        textEdit->setFocus();
		pressPos = event->pos();
		state = ShapeState::moving;
		event->accept();
		auto win = (WinBase*)parent();
		win->canvas->changeShape(this,true);
		win->update();
	}
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this,true);
    }
    event->accept();
}
void ShapeText::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        auto p = textEdit->pos() + span;
		textEdit->move(p.x(), p.y());
        pressPos = pos;
        auto win = (WinBase*)parent();
        win->canvas->update();
        event->accept();
    }
}
