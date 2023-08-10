#pragma once
#include "Shape.h"
namespace Shape {
	class Eraser : public Shape
	{
	public:
		Eraser();
		~Eraser();
		void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
		bool EndDraw() override;		
		double strokeWidth;
	private:
		std::vector<BLPoint> points;
	};
}

