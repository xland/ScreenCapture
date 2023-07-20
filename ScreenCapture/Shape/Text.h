#pragma once
#include "Shape.h"
#include "blend2d.h"
namespace Shape {
	class Text : public Shape
	{
	public:
		void Draw(BLContext* context) override {};
		double rotateDeg = 0.0;
		bool isFill = false;
		BLRgba32 color;
	};
}

