#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_WindowMain.h"

class WindowMain : public QMainWindow
{
    Q_OBJECT

public:
    WindowMain(QWidget *parent = nullptr);
    ~WindowMain();
protected:
    void paintEvent(QPaintEvent* event) override;
private:
private:
    std::vector<uchar> bgPix;
    QImage* bgImg;
    int x, y, w, h;
    Ui::WindowMainClass ui;
};
