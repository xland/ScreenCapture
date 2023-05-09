#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ScreenCapture.h"

class ScreenCapture : public QMainWindow
{
    Q_OBJECT

public:
    ScreenCapture(QWidget *parent = nullptr);
    ~ScreenCapture();

private:
    Ui::ScreenCaptureClass ui;
};
