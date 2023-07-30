#pragma once
#include "Shape.h"
#include "blend2d.h"
#include <string>
namespace Shape {
	class Text : public Shape
	{
	public:
		void Draw(BLContext* context, const double& x1, const double& y1, const double& x2, const double& y2);
		void SetIndex(const double& x);
		void InsertWord(const std::wstring& word);
		double rotateDeg = 0.0;
		bool isFill = false;
		BLRgba32 color;
		BLBox box{ -1.0f, -1.0f, -1.0f, -1.0f };
		std::wstring text;
		double fontSize = 158.0f;
		bool showInputCursor = true;
		int cursorIndex = 0;
		bool isEnding = false;
		double margin = 18.0f;
	};
}

