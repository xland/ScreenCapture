#pragma once

#include <QObject>
#include <qimage.h>

class LayerBase  : public QObject
{
	Q_OBJECT

public:
	LayerBase(QObject *parent);
	virtual ~LayerBase();
	virtual void paint(QPainter* painter) = 0;
public:
	QImage img;
};
