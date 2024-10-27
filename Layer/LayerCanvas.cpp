#include "LayerCanvas.h"
#include "../Win/WinFull.h"

LayerCanvas::LayerCanvas(QObject *parent) : LayerBase(parent)
{
	auto winFull = WinFull::get();
	img = QImage(winFull->w, winFull->h, QImage::Format_ARGB32);
}

LayerCanvas::~LayerCanvas()
{
}

void LayerCanvas::paint(QPainter * painter)
{
}
