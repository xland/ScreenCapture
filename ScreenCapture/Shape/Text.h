#pragma once
#include "Shape.h"
#include "blend2d.h"
#include <string>
#include <Windows.h>
namespace Shape {
	class Text : public Shape
	{
	public:
		Text();
		~Text();
		void Draw(const double& x1, const double& y1, const double& x2, const double& y2) override;
		void ShowDragger() override;
		bool EndDraw() override;
		void SetIndex(const double& x);
		void MouseInDragger(const double& x, const double& y) override;
		void DragDragger(const double& x, const double& y) override;
		void DeleteWord();
		void InsertWord(const std::wstring& word);
		void activeKeyboard(LONG x, LONG y);
		double rotateDeg = 0.0;
		bool isFill = false;
		BLRgba32 color;
		std::wstring text;
		double fontSize;
		bool showInputCursor = true;
		int cursorIndex = 0;
		bool onlyDrawText = false;
		double margin = 18.0f;
		BLBox draggers[4];
	private:
		double tempDraggerX, tempDraggerY;
		int draggerIndex = -1;
		BLBox box{ -1.0f, -1.0f, -1.0f, -1.0f };
		bool isDraggingDragger = false;
	};
}

