#include "Shape.h"
#include "../MainWin.h"
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