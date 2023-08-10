#pragma once
#include "blend2d.h"
#include "Shape.h"
namespace Shape {
	class Arrow :public Shape
	{
	public:
		Arrow();
		~Arrow();
		void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
		bool EndDraw() override;
		void ShowDragger() override;
		void MouseInDragger(const double& x, const double& y) override;
		void DragDragger(const double& x, const double& y) override;
		BLPath path;
		bool isFill = false;
		bool isEraser = false;
		bool isTansparent = false;
		double strokeWidth;
		BLRgba32 color; 
	private:
		BLBox draggers[2];
		double tempDraggerX, tempDraggerY;
		int draggerIndex = -1;
	};
}

