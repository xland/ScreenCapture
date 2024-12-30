#include <QPainter>
#include <Windows.h>


#include "WinBoard.h"
#include "WinBase.h"
#include "WinBox.h"
#include "../Shape/ShapeBase.h"
#include "../Tool/ToolMain.h"

WinBoard::WinBoard(QObject *parent) : WinBase(parent)
{
    auto win = (WinBox*)parent;
    x = win->x; y = win->y;
    w = win->w; h = win->h;
    initWindow();
    show();
}

WinBoard::~WinBoard()
{
}

void WinBoard::refresh(bool releaseFlag)
{
    auto win = (WinBox*)parent();
    initImg();
    img.fill(Qt::transparent);
    QPainter p(&img);
    if (win->padding > 0) {
        p.setClipRect(win->padding, win->padding, w - 2 * win->padding, h - 2 * win->padding);
    }
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    bool enableRedo{ false };
    bool enableUndo{ false };
    for (auto shape : win->shapes)
    {
        if (shape->state == ShapeState::hidden) {
			enableRedo = true;
        }
        else
        {
            if (shape->isEraser || shape->state == ShapeState::ready) {
                shape->paint(&p);
                enableUndo = true;
            }
        }
    }
    paint();
    p.end();
    if (releaseFlag) {
        releaseImg();
        win->toolMain->setBtnEnable(enableUndo, enableRedo);
    }
}
