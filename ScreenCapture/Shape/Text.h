#pragma once
#include "Shape.h"
#include "blend2d.h"
#include <string>
namespace Shape {
	class Text : public Shape
	{
	public:
		void Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2);
		double rotateDeg = 0.0;
		bool isFill = false;
		BLRgba32 color;
		double startX = -1;
		double startY = -1;
		double endX = -1;
		double endY = -1;
		std::string text;
		bool showInputCursor = true;
	};
}

