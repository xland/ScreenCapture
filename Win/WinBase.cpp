#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "Shape/ShapeBase.h"
#include "WinBase.h"
#include "WinPin.h"
#include "App/Util.h"
#include "Canvas.h"

WinBase::WinBase(QWidget* parent) : QMainWindow(parent)
{
}
WinBase::~WinBase()
{
}

void WinBase::saveToClipboard()
{
    auto img = getTargetImg();
    Util::imgToClipboard(img);
    close();
    qApp->exit(9);
}

void WinBase::saveToFile()
{
    auto img = getTargetImg();
    auto flag = Util::saveToFile(img);
    if (flag) {
        close();
        qApp->exit(8);
    }
}

void WinBase::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape) {
        qApp->exit(3);
    }
    else if (event->key() == Qt::Key_Left) {
        moveCursor(QPoint(-1, 0));
    }
    else if (event->key() == Qt::Key_Up) {
        moveCursor(QPoint(0,-1));
	}
	else if (event->key() == Qt::Key_Right) {
		moveCursor(QPoint(1, 0));
	}
	else if (event->key() == Qt::Key_Down) {
		moveCursor(QPoint(0, 1));
	}
    else if (event->key() == Qt::Key_Delete) {
        canvas->removeShapeHover();
    }
    else if (event->key() == Qt::Key_Backspace) {
        canvas->removeShapeHover();
    }
    else if (event->modifiers() & Qt::ControlModifier) {
        if (event->key() == Qt::Key_Z) {
            canvas->undo();
        }
        else if (event->key() == Qt::Key_Y) {
            canvas->redo();
        }
        else if (event->key() == Qt::Key_S) {
            saveToFile();
        }
        else if (event->key() == Qt::Key_C) {
            saveToClipboard();
        }
        else if (event->key() == Qt::Key_R) {
            canvas->copyColor(0);
        }
        else if (event->key() == Qt::Key_H) {
            canvas->copyColor(1);
        }
        else if (event->key() == Qt::Key_K) {
            canvas->copyColor(2);
        }
        else if (event->key() == Qt::Key_P) {
            canvas->copyColor(3);
        }
    }
}

void WinBase::moveCursor(const QPoint& pos)
{
    auto pos1 = QCursor::pos()+pos;
    QCursor::setPos(pos1);
}

void WinBase::mouseDoubleClickEvent(QMouseEvent* event)
{
    saveToClipboard();
}
