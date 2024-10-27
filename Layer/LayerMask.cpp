#include "LayerMask.h"
#include "../Win/WinFull.h"

LayerMask::LayerMask(QObject *parent) : LayerBase(parent)
{
	auto winFull = WinFull::get();
	img = QImage(winFull->w, winFull->h, QImage::Format_ARGB32);
}

LayerMask::~LayerMask()
{
}

void LayerMask::paint(QPainter* painter)
{

}
