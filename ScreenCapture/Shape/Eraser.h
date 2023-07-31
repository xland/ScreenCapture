#pragma once
#include "Shape.h"
namespace Shape {
	class Eraser : public Shape
	{
	public:
		void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;

		BLPath path;
		double strokeWidth;
	};
}

