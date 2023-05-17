#ifndef PATHMODEL_H
#define PATHMODEL_H

#include <QPainterPath>
#include <QColor>
#include <QBrush>

class PathModel : public QPainterPath
{
public:
    explicit PathModel();
    bool needFill = false;
    bool needBorder = true;
    int borderWidth = 2;
    QColor borderColor = Qt::red;
    QColor fillColor = Qt::transparent;
signals:

};

#endif // PATHMODEL_H
