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