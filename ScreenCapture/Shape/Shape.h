#pragma once
#include <vector>
#include "blend2d.h"
#include "../Painter.h"
namespace Shape {
	class Shape
	{
	public:
		Shape();
		virtual ~Shape();
		virtual void Draw(const double& x1 = -1.f,const double& y1 = -1.f,const double& x2=-1.f,const double& y2=-1.f) {};
		virtual void ShowDragger() {}; 
		virtual bool EndDraw() { return true; };
		virtual void MouseInDragger(const double& x, const double& y) {};
		virtual void DragDragger(const double& x, const double& y) {};
		bool isTemp = true;
		bool needDraw = true;
	protected:
		int draggerSize = 6;
	};
}


