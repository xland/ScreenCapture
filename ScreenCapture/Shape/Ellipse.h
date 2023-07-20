#pragma once
#include "Shape.h"
namespace Shape {
	class Ellipse : public Shape
	{
	public:
		void Draw(BLContext* context, double x1, double y1, double x2, double y2) override {};
	};
}

