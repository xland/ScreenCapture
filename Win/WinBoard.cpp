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
    initSizeByWin(win);
    initWindow();
    show();
}

WinBoard::~WinBoard()
{
}

void WinBoard::refresh(bool releaseFlag)
{
    initImg();
    img.fill(Qt::transparent);
    QPainter p(&img);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setRenderHint(QPainter::TextAntialiasing, true);
    auto win = (WinBox*)parent();
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
