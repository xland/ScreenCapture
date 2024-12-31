#pragma once

#include <QWidget>
#include <QTextEdit>
#include <QVBoxLayout>

class About : public QWidget {

    Q_OBJECT
public:
    About(QWidget* parent = nullptr);
protected:
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
};
