#pragma once

#include <QObject>
#include "LayerBase.h"

class LayerCanvas  : public LayerBase
{
	Q_OBJECT
public:
	LayerCanvas(QObject *parent);
	~LayerCanvas();
	void paint(QPainter* painter) override;
private:
};
