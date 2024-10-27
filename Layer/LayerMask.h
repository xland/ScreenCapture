#pragma once

#include <QObject>
#include "LayerBase.h"

class LayerMask  : public LayerBase
{
	Q_OBJECT

public:
	LayerMask(QObject *parent);
	~LayerMask();
	void paint(QPainter* painter) override;
};
