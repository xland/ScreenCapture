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
    QColor currentColor();
protected:
    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
signals:
    void colorChanged();
private:
    QList<QColor> colors;
    QList<QRectF> rects;
    int hoverIndex = -1;
    Ui::ColorSelector* ui;
};

#endif // COLORSELECTOR_H
