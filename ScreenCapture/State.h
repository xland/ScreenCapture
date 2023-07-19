#pragma once

enum class State
{
	start = 0,
	maskReady,
    rect,
    ellipse,
    arrow,
    pen,
    line,
    mosaic,
    text,
    number,
    eraser,
	lastPathDrag
};