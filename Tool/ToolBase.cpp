#include <qlayout.h>
#include <qpushbutton.h>
#include <qtooltip.h>

#include "App/Util.h"
#include "App/App.h"
#include "Win/WinBase.h"
#include "Win/Canvas.h"
#include "ToolBase.h"
#include "ToolSub.h"

ToolBase::ToolBase(QWidget* parent) : QWidget(parent)
{

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

void ToolBase::initWindow()
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
    setFocusPolicy(Qt::StrongFocus);
}

void ToolBase::showEvent(QShowEvent* event)
{
    QWidget::showEvent(event);
    //用qt的setFocus始终无法使ToolSub聚焦
    auto wid = (HWND)winId();
    SetFocus(wid);
}

void ToolBase::keyPressEvent(QKeyEvent* event)
{
    auto win = (WinBase*)parent();
    win->keyPressEvent(event);
}