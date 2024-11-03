#include <qpainter.h>
#include <QTransform>
#include <QTextDocument>
#include <numbers>
#include <QTextCharFormat>

#include "ShapeText.h"
#include "../App/App.h"
#include "../Tool/ToolSub.h"
#include "../Win/WinBase.h"
#include "../Layer/Canvas.h"


ShapeText::ShapeText(QObject* parent) : ShapeBase(parent)
{
    auto win = (WinBase*)parent;
    draggers.push_back(QRect());
    draggers.push_back(QRect());
    draggers.push_back(QRect());
    draggers.push_back(QRect());

    color = win->toolSub->getColor();
    fontSize = win->toolSub->getStrokeWidth();
    bold = win->toolSub->getSelectState("bold");
    italic = win->toolSub->getSelectState("italic");

    textEdit = new QTextEdit((QWidget*)parent);
    textEdit->setLineWrapMode(QTextEdit::NoWrap);
    textEdit->setContentsMargins(10, 10, 10, 10);
    textEdit->setStyleSheet("border: 1px solid gray; border-radius: 2px;");
    QPalette palette = textEdit->palette();
    palette.setColor(QPalette::Base, Qt::transparent); // 设置背景色为透明
    textEdit->setPalette(palette);
    textEdit->setFontItalic(italic);
    textEdit->setFontPointSize(fontSize);
    if (bold) {
        QFont font = textEdit->font();
        font.setWeight(QFont::Bold);
        textEdit->setFont(font);
    }
    textEdit->hide();
    connect(textEdit->document(), &QTextDocument::contentsChanged, this, &ShapeText::adjustSize);
}

ShapeText::~ShapeText()
{
    delete textEdit;
}

void ShapeText::resetDragger()
{
    auto half{ draggerSize / 2 };
    draggers[0].setRect(startPos.x() - half, startPos.y() - half, draggerSize, draggerSize);
    draggers[1].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
    draggers[2].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
    draggers[3].setRect(endPos.x() - half, endPos.y() - half, draggerSize, draggerSize);
}

void ShapeText::adjustSize()
{
    textEdit->document()->adjustSize();
    auto size = textEdit->document()->size().toSize();
    size += QSize(20, 20); // 增加边距
    textEdit->setFixedSize(size);
}

void ShapeText::paint(QPainter* painter)
{
    //painter->save();
    //painter->translate(startPos);
    //painter->rotate(rotate);
    //textEdit->render(painter);
    //textEdit->setParent((QWidget*)painter->device());
    //painter->restore();

}
void ShapeText::paintDragger(QPainter* painter)
{
    painter->setPen(QPen(QBrush(QColor(0, 0, 0)), 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawRect(draggers[0]);
    painter->drawRect(draggers[1]);
}
void ShapeText::mouseMove(QMouseEvent* event)
{
    if (state != ShapeState::ready) return;
    auto pos = event->pos();
    hoverDraggerIndex = -1;
    if (draggers[0].contains(pos)) {
        hoverDraggerIndex = 0;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[1].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[2].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    else if (draggers[3].contains(pos)) {
        hoverDraggerIndex = 1;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::SizeAllCursor);
    }
    if (hoverDraggerIndex == -1) {
        //hoverDraggerIndex = 8;
        auto win = (WinBase*)parent();
        win->updateCursor(Qt::IBeamCursor);
    }
    if (hoverDraggerIndex > -1) {
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this);
        event->accept();
    }
}
void ShapeText::mousePress(QMouseEvent* event)
{
    if (state == ShapeState::temp) {
        startPos = event->pos().toPointF();
        endPos = startPos;

        textEdit->move(event->pos() + QPoint(-10,-10));
        textEdit->setText("");
        textEdit->show();
        textEdit->setFocus();
    }
}
void ShapeText::mouseRelease(QMouseEvent* event)
{
    if (state >= ShapeState::sizing0) {
        resetDragger();
        QRect rect;
        rect.setCoords(startPos.x(), startPos.y(), endPos.x(), endPos.y());
        //textEdit->move(startPos.toPoint());
        //textEdit->setFixedSize(rect.width(), rect.height());
        //textEdit->setFocus();
        ////textEdit->setPlainText("这是我的文本");
        //textEdit->setHtml("<p style='font-style: italic; text-shadow: 2px 2px 5px gray;'>这是一段带阴影和斜体的文本。</p>");
        //textEdit->raise();
        state = ShapeState::ready;
        auto win = (WinBase*)parent();
        win->canvas->changeShape(this,true);
        event->accept();
    }
}
void ShapeText::mouseDrag(QMouseEvent* event)
{
    if (state == ShapeState::ready) {
        return;
    }
    if (state == ShapeState::sizing0) {
        startPos = event->pos();
    }
    if (state == ShapeState::sizing1) {
        endPos = event->pos();
    }
    if (state == ShapeState::sizing2) {
        endPos = event->pos();
    }
    else if (state == ShapeState::moving) {
        auto pos = event->pos();
        auto span = pos - pressPos;
        startPos+=span;
        endPos+=span;
        pressPos = pos;
    }
    auto win = (WinBase*)parent();
    win->canvas->update();
    event->accept();
}
