#pragma once
#include "Shape.h"
namespace Shape {
	class Box : public Shape
	{
		public:
			Box();
			~Box();
			void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
			void ShowDragger() override;
			void MouseInDragger(const double& x, const double& y) override;
			void DragDragger(const double& x, const double& y) override;
			bool EndDraw() override;
			BLBox box;
			BLRgba32 color;
			double strokeWidth;
		private:
			BLBox draggers[4];
			double tempDraggerX, tempDraggerY;
			int draggerIndex = -1;
	};
}

