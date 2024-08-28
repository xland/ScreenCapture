#pragma once

#include <functional>
#include <include/core/SkCanvas.h>

enum class EventType {
	maskReady = 0,
    undoDisable,
    redoDisable,
    unselectAllTool
};

enum class State
{
    start = 0,
    mask,
    tool,
    rect,
    ellipse,
    arrow,
    number,
    pen,
    line,
    text,
    mosaic,
    eraser
};

using MouseEventCB = std::function<void(const int&, const int&)>;
using CustomEventCB = std::function<void(const EventType& type, const uint32_t& msg)>;
using PaintEventCB = std::function<void(SkCanvas* canvas)>;
