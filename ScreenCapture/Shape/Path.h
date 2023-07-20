#pragma once
#include "blend2d.h"
#include "Shape.h"
namespace Shape {
	class Path :public Shape
	{
	public:
		Path(const BLRect& rect,const BLRgba32& color,const bool& isFill);
		~Path();
		void Draw(BLContext* context, double x1, double y1, double x2, double y2) override {};
		BLPath path;
		bool isFill = false;
		bool isEraser = false;
		bool isTansparent = false;
		BLRgba32 color;
	private:
		void rectToPath(const BLRect& rect);
	};
}

