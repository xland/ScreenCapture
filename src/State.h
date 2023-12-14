#pragma once

enum class State
{
    start = 0,
    mask,
    tool,
    box,
    ellipse,
    arrow,
    number,
    pen,
    line,
    text,
    image,
    mosaic,
    eraser
};
State state = State::start;