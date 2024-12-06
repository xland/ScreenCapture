#include <Windows.h>

#include "WinBaseLayer.h"
#include "WinBase.h"

WinBaseLayer::WinBaseLayer(QWidget* parent) : QWidget(parent)
{
}

WinBaseLayer::~WinBaseLayer()
{
}
void WinBaseLayer::init(WinBase* father)
{
    this->father = father;
    initWindow();
}

void WinBaseLayer::showEvent(QShowEvent * event)
{
    
    //QGuiApplication::primaryScreen()

    auto hwnd = (HWND)winId();
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowPos(hwnd, nullptr, 
        father->x, father->y, father->w, father->h, 
        SWP_NOZORDER | SWP_SHOWWINDOW);
    QWidget::showEvent(event);
}
void WinBaseLayer::initWindow()
{
    setAutoFillBackground(false);
    setMouseTracking(false);
    setAttribute(Qt::WA_OpaquePaintEvent);
    setAttribute(Qt::WA_NoSystemBackground);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setAttribute(Qt::WA_TransparentForMouseEvents, true);
    setWindowState(Qt::WindowFullScreen);
    setFixedSize(father->w, father->h);
    show();
    auto hwnd = (HWND)winId();
    SetWindowLong(hwnd, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT);
    SetWindowPos(hwnd, nullptr, father->x, father->y, father->w, father->h, SWP_NOZORDER | SWP_SHOWWINDOW);
}
