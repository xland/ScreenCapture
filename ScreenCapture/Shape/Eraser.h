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
		void ShowDragger() override;
		void MouseInDragger(const double& x, const double& y) override;
		void DragDragger(const double& x, const double& y) override;
		void CopyCanvasImg();
		double strokeWidth;
		BLBox box;
		BLImage* canvasImgCopy = nullptr;
	private:
		std::vector<BLPoint> points;
		BLBox draggers[4];
		double tempDraggerX, tempDraggerY;
		int draggerIndex = -1;
	};
}

