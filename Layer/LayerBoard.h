#pragma once

#include <QObject>
#include <qpixmap.h>
#include "LayerBase.h"

class LayerBoard  : public LayerBase
{
	Q_OBJECT
public:
	LayerBoard(QObject *parent);
	~LayerBoard();
	void paint(QPainter* painter) override;
};
