#pragma once
#include "Shape.h"
namespace Shape {
	class Ellipse : public Shape
	{
	public:
		Ellipse();
		~Ellipse();
		void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
		void ShowDragger() override;
		void MouseInDragger(const double& x, const double& y) override;
		bool EndDraw() override;
		void DragDragger(const double& x, const double& y) override;		
		bool isTansparent = false;//todo
		BLRgba32 color;
		double strokeWidth;
		BLBox box;
	private:
		BLBox draggers[4];
		int draggerIndex = -1;
		double tempDraggerX, tempDraggerY;
	};
}

