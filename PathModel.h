#ifndef PATHMODEL_H
#define PATHMODEL_H

#include <QPainterPath>
#include <QColor>
#include <QBrush>

class PathModel : public QPainterPath
{
public:
    explicit PathModel();
    void resetPoint5();

    bool needFill = false;
    bool needBorder = true;
    bool needPaint = true;
    bool needDelete = true;
    bool isEraser;


    qreal borderWidth = 2.0;
    QColor borderColor = Qt::red;
    QColor fillColor = Qt::transparent;
signals:

};

#endif // PATHMODEL_H
