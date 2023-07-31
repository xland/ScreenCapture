#pragma once
#include "Shape.h"
namespace Shape {
	class Box : public Shape
	{
		public:
			BLBox box;
			BLBox boxes[4];
			bool isFill = false;
			double rotateDeg = 0.0;
			bool isTansparent = false;//todo
			BLRgba32 color;
			double strokeWidth;
			void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
			virtual void ShowDragger(BLContext* context);
		private:
	};
}

