#ifndef BUTTONDOT_H
#define BUTTONDOT_H

#include <QPushButton>
#include <QObject>
#include <QWheelEvent>

class ButtonDot : public QPushButton
{
    Q_OBJECT
public:
    ButtonDot(QWidget* parent = nullptr);
    void setFontSize(int size);
    int size = 2;
protected:
    void wheelEvent(QWheelEvent* event) override;
};

#endif // BUTTONDOT_H
