#include <Windows.h>
#include <windowsx.h>
#include <dwmapi.h>

#include "Shape/ShapeBase.h"
#include "WinBase.h"
#include "WinPin.h"
#include "../App/Util.h"

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
}

void WinBase::saveToFile()
{
    auto img = getTargetImg();
    Util::saveToFile(img);
    close();
}
