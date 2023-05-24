#ifndef BUTTONBOT_H
#define BUTTONBOT_H

#include <QPushButton>
#include <QObject>
#include <QWheelEvent>

class ButtonBot : public QPushButton
{
    Q_OBJECT
public:
    ButtonBot(QWidget* parent = nullptr);
    int size = 2;
protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // BUTTONBOT_H
