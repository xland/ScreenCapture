#ifndef COLORSELECTOR_H
#define COLORSELECTOR_H

#include <QWidget>

namespace Ui
{
class ColorSelector;
}

class ColorSelector : public QWidget
{
    Q_OBJECT

public:
    explicit ColorSelector(QWidget* parent = nullptr);
    ~ColorSelector();
//    bool mousePress(QMouseEvent* mouseEvent);
//    bool mouseRelease(QMouseEvent* mouseEvent);
protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
private:
    QVector<QColor> colors;
    QVector<QRectF> rects;
    int hoverIndex = -1;
    Ui::ColorSelector* ui;
};

#endif // COLORSELECTOR_H
