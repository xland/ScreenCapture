#pragma once

enum class State
{
	start = 0,
	maskReady,
    rect,
    ellipse,
    arrow,
    number,
    pen,
    line,
    text,
    mosaic,
    eraser,
	lastPathDrag
};