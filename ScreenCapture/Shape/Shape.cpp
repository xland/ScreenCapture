#include "Shape.h"
#include "../WindowBase.h"
namespace Shape{

	static std::vector<Shape> history;

	Shape::Shape()
	{

	}
	Shape::~Shape()
	{

	}
	std::vector<Shape> getHistory()
	{
		return history;
	}
}