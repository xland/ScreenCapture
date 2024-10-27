#include <QPainter>

#include "LayerBoard.h"
#include "../Win/WinFull.h"

LayerBoard::LayerBoard(QObject *parent) : LayerBase(parent)
{
	auto winFull = WinFull::get();
	img = winFull->bgImg;
}

LayerBoard::~LayerBoard()
{
}

void LayerBoard::paint(QPainter* painter)
{
	painter->drawImage(WinFull::get()->rect(), img);
}
