#pragma once

#include <functional>

enum class EventType {
	maskReady = 0,
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
