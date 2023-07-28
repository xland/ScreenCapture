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
    mosaic,
    text,
    eraser,
	lastPathDrag
};