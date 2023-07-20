#pragma once
#include "Shape.h"
#include "blend2d.h"
namespace Shape {
	class Text : public Shape
	{
	public:
		void Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2) override {};
		double rotateDeg = 0.0;
		bool isFill = false;
		BLRgba32 color;
	};
}

