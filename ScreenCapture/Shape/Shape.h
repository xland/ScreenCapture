#pragma once
#include <vector>
#include "blend2d.h"
#include "../State.h"
namespace Shape {
	class Shape
	{
	public:
		Shape();
		virtual ~Shape();
		virtual void Draw([[maybe_unused]] const double& x1 = -1.f, 
			[[maybe_unused]] const double& y1 = -1.f, 
			[[maybe_unused]] const double& x2=-1.f, 
			[[maybe_unused]] const double& y2=-1.f) {};
		virtual void ShowDragger() {}; 
		virtual bool EndDraw() { return true; };
		virtual void MouseInDragger([[maybe_unused]] const double& x, [[maybe_unused]] const double& y) {};
		virtual void DragDragger([[maybe_unused]] const double& x, [[maybe_unused]] const double& y) {};
		bool isTemp = true;
		bool needDraw = true;
		State state;
		bool isFill = false;
	protected:
		int draggerSize = 6;
	};
}


