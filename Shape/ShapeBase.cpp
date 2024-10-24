#include "ShapeBase.h"

ShapeBase::ShapeBase(const QPoint& pos,QObject* parent):QObject(parent), startPos{pos}
{
    // setAttribute(Qt::WA_TransparentForMouseEvents, true);
}

ShapeBase::~ShapeBase()
{
}
