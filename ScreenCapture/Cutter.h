
#ifndef CUTTER_H
#define CUTTER_H


#include <QObject>
#include "ScreenShoter.h"

class Cutter : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int absoluteX READ absoluteX CONSTANT)
    Q_PROPERTY (int absoluteY READ absoluteY CONSTANT)
    Q_PROPERTY (int totalWidth READ totalWidth CONSTANT)
    Q_PROPERTY (int totalHeight READ totalHeight CONSTANT)
public:
    explicit Cutter(QObject *parent = nullptr);
    int absoluteX() const { return ScreenShoter::Get()->absoluteX; }
    int absoluteY() const { return ScreenShoter::Get()->absoluteY; }
    int totalWidth() const { return ScreenShoter::Get()->totalWidth / 1.5; } //todo
    int totalHeight() const { return ScreenShoter::Get()->totalHeight / 1.5; }//todo
signals:

};

#endif // CUTTER_H
