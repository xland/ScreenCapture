#pragma once
enum class State
{
	Start = 0,
	MaskReady,
	RectEllipse,
	Arrow,
	Pen,
	Mosaic,
	Text,
	Eraser,
	LastPathDrag
};
struct Rect
{
	int x;
	int y;
	int w;
	int h;
}; 
struct Point
{
	int x;
	int y;
};