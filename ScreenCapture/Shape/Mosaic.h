#pragma once
#include "Shape.h"
#include "blend2d.h"
namespace Shape {
	class Mosaic : public Shape
	{
		public:
			Mosaic();
			~Mosaic();
			void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
			bool EndDraw() override;
			void ShowDragger() override;
			void MouseInDragger(const double& x, const double& y) override;
			void DragDragger(const double& x, const double& y) override;
			void InitMosaicImg();
			BLBox box;
			int strokeWidth;
			bool isFill = false;
		private:
			void setSamplingPoints(BLPointI* points,const int& x, const int& y);
			BLBox draggers[4];
			double tempDraggerX, tempDraggerY;
			int draggerIndex = -1;
			std::vector<BLPoint> points;
			BLImage* mosaicPatchImg = nullptr;
	};
}

