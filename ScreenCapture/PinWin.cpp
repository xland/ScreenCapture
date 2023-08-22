#include "PinWin.h"


PinWin::PinWin(const int& x, const int& y, BLImage* img)
{
	this->x = x;
	this->y = y;
	this->w = img->width();
	this->h = img->height();
	OriginalImage = img;
	InitLayerImg();
	InitWindow();
	Show();
}
PinWin::~PinWin()
{

}

int PinWin::OnHitTest() {
	return HTCAPTION;
}