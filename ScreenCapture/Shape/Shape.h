#pragma once
#include <vector>
#include "blend2d.h"
namespace Shape {
	class Shape
	{
	public:
		Shape();
		~Shape();
		virtual void Draw(BLContext* context) {};
	};
}


