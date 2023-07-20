#pragma once
#include "blend2d.h"
#include "Shape.h"
namespace Shape {
	class Path :public Shape
	{
	public:
		Path(const BLRect& rect,const BLRgba32& color,const bool& isFill);
		~Path();
		BLPath path;
		bool isFill = false;
		BLRgba32 color;
	private:
		void rectToPath(const BLRect& rect);
	};
}

