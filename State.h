#pragma once

#define WM_REFRESH (WM_APP+1)
#define WM_SHOW_DRAGGER (WM_APP+2)
#define WM_HIDE_DRAGGER (WM_APP+3)
#define WM_FLASH_CURSOR (WM_APP+4)

enum class State
{
    start = 0,
    mask,
    tool,
    rect,
    ellipse,
    arrow,
    number,
    line,
    text,
    mosaic,
    eraser
};
