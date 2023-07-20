#pragma once
#include <vector>
#include "blend2d.h"
namespace Shape {
	class Shape
	{
	public:
		Shape();
		~Shape();
		virtual void Draw(BLContext* context,
			const double& x1,const double& y1,const double& x2=0.f,const double& y2=0.f) {};
		bool isTemp = true;
	};
}


