#pragma once
#include <vector>
#include "blend2d.h"
namespace Shape {
	class Shape
	{
	public:
		Shape();
		~Shape();
		virtual void Draw(BLContext* context,double x1,double y1,double x2=0.f,double y2=0.f) {};
		bool isTemp = true;
	};
}


