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

    bool needFill;
    bool needPaint = true;
    bool needDelete = true;
    bool isEraser = false;
    bool isMosaic = false;


    qreal borderWidth = 2.0;
    QColor color;
    qreal mosaicRectSize = 6.0;
    QImage* mosaicBg;
signals:

};

#endif // PATHMODEL_H
