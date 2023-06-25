#pragma once
#include "nativeui/nativeui.h"
class MainWin : public nu::Window
{
public:
	MainWin();

private:
	void paint(nu::Painter* painter);
	nu::RectF rect;
	double scaleFactor;
	scoped_refptr<nu::Container> container;
};

