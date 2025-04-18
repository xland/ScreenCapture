﻿#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "../App/Util.h"
#include "../App/App.h"
#include "../Win/WinFull.h"
#include "../Win/WinBase.h"
#include "ToolBase.h"
#include "ToolSub.h"

ToolBase::ToolBase(QWidget* parent) : QWidget(parent)
{
    setMouseTracking(true);
    setVisible(false);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_Hover);
    setCursor(Qt::PointingHandCursor);
    //setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setWindowFlag(Qt::WindowDoesNotAcceptFocus);   //右键Pin菜单，要点两次才会出现
}
ToolBase::~ToolBase()
{
}

void ToolBase::btnCheckChange(BtnCheck* btn)
{
}

void ToolBase::btnClick(Btn* btn)
{
}

void ToolBase::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    //用qt的setFocus始终无法使ToolSub聚焦
    auto wid = (HWND)winId();
    SetFocus(wid);
}

std::shared_ptr<QPainter> ToolBase::getPainter()
{
    auto font = Util::getIconFont(15);
    font->setPixelSize(15);
    auto painter = std::make_shared<QPainter>(this);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->setRenderHint(QPainter::TextAntialiasing, true);
    painter->setFont(*font);
    QPen pen;
    pen.setColor(QColor(22, 118, 255));
    pen.setWidthF(border);
    painter->setPen(pen);
    painter->setBrush(Qt::white);
    return painter;
}

void ToolBase::paintBtn(const QChar& icon, const QColor& frontColor, const QColor& bgColor, QRect& rect,QPainter* p)
{
    rect.adjust(2, 4, -2, -4);
    p->setPen(Qt::NoPen);
    p->setBrush(bgColor);
    p->drawRoundedRect(rect, 6, 6);
    p->setPen(frontColor);
    p->drawText(rect, Qt::AlignCenter, icon);
}
void ToolBase::keyPressEvent(QKeyEvent* event)
{
    /*event->ignore();
    auto win = (WinBase*)parent();
    if (event->key() == Qt::Key_Escape) {
        win->escPress();
    }
    else if (event->key() == Qt::Key_Delete) {
        win->removeShape();
    }
    else if (event->key() == Qt::Key_Left) {
        win->moveByKey(0);
    }
    else if (event->key() == Qt::Key_Up) {
        win->moveByKey(1);
    }
    else if (event->key() == Qt::Key_Right) {
        win->moveByKey(2);
    }
    else if (event->key() == Qt::Key_Down) {
        win->moveByKey(3);
    }
    else if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_T) {
            win->hideTools();
        }
        else if (event->key() == Qt::Key_Z) {
            win->undo();
        }
        else if (event->key() == Qt::Key_Y) {
            win->redo();
        }
        else if (event->key() == Qt::Key_S) {
            win->saveToFile();
        }
        else if (event->key() == Qt::Key_C) {
            win->saveToClipboard();
        }
        else if (event->key() == Qt::Key_H) {
            win->copyColor(0);
        }
        else if (event->key() == Qt::Key_R) {
            win->copyColor(1);
        }
        else if (event->key() == Qt::Key_K) {
            win->copyColor(2);
        }
        else if (event->key() == Qt::Key_P) {
            win->copyColor(3);
        }
    }*/
}