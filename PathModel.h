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
    void initMosaicBg(qreal scaleFactor, QImage* curImg);

    bool needFill = false;
    bool needBorder = true;
    bool needPaint = true;
    bool needDelete = true;
    bool isEraser;
    bool isMosaic = false;


    qreal borderWidth = 2.0;
    QColor borderColor = Qt::red;
    QColor fillColor = Qt::transparent;
    qreal mosaicRectSize = 6.0;
    QImage* mosaicBg;
signals:

};

#endif // PATHMODEL_H
