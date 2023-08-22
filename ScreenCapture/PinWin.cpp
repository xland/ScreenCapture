#include "PinWin.h"


PinWin::PinWin(const int& x, const int& y, BLImage* img)
{
	this->x = x;
	this->y = y;
	w = img->width();
	h = img->height();
	OriginalImage = img;
	InitLayerImg();
	InitWindow(true);
}
PinWin::~PinWin()
{

}

int PinWin::OnHitTest() {
	return HTCAPTION;
}