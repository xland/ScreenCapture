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
			BLImageData* bgImgData;
			BLImageData* canvasImgData;
			float screenW, screenH;
			BLBox box;
			bool isFill = false;
			double rotateDeg = 0.0;
			bool isTansparent = false;//todo
			BLRgba32 color;
			double strokeWidth;
		private:
			void drawMosaic(BLContext* context);
			BLBox draggers[4];
			double tempDraggerX, tempDraggerY;
			int draggerIndex = -1;
	};
}

