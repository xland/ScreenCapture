#ifndef PATHMODEL_H
#define PATHMODEL_H

#include <QPainterPath>
#include <QColor>
#include <QBrush>
#include <QImage>

class PathModel : public QPainterPath
{
public:
    explicit PathModel();
    void resetPoint5();
    void initPatch(QImage* bgImg, qreal scaleFactor);

    bool needFill;
    bool needPaint = true;
    bool needDelete = true;
    bool isEraser = false;
    bool isMosaic = false;
    bool isEllipse = false;


    qreal borderWidth = 2.0;
    QColor color;
    QPoint patchPosition;
    QImage patchImg;
signals:

};

#endif // PATHMODEL_H
