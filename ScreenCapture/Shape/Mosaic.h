#pragma once
#include "Shape.h"
#include "blend2d.h"
namespace Shape {
	class Mosaic : public Shape
	{
		public:
			Mosaic();
			~Mosaic();
			BLImageData* bgImgData;
			BLImageData* canvasImgData;
			float screenW, screenH;
			BLBox box;
			bool isFill = false;
			double rotateDeg = 0.0;
			bool isTansparent = false;//todo
			BLRgba32 color;
			double strokeWidth;
			void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
		private:
	};
}

