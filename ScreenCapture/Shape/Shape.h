#pragma once
#include <vector>
namespace Shape {
	class Shape
	{
	public:
		Shape();
		~Shape();
		std::vector<Shape> getHistory();
	};
}


