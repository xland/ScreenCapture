#pragma once

enum class State
{
	start = 0,
	maskReady,
    rect,
    ellipse,
    arrow,
    number,
    line,
    mosaic,
    pen,
    text,
    eraser,
	lastPathDrag
};