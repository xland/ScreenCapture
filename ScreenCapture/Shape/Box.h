#pragma once
#include "Shape.h"
namespace Shape {
	class Box : public Shape
	{
		public:
			BLBox box;
			bool isFill = false;
			double rotateDeg = 0.0;
			bool isTansparent = false;//todo
			BLRgba32 color;
			double strokeWidth;
			void Draw(BLContext* context, double x1, double y1, double x2, double y2) override;
		private:
	};
}

