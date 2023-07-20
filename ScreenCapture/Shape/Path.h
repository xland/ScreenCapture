#pragma once
#include "blend2d.h"
#include "Shape.h"
namespace Shape {
	class Path :public Shape
	{
	public:
		Path();
		~Path();
		BLPath path;
		bool isFill = false;
		BLRgba32 color;
	};
}

