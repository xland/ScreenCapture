#pragma once

enum class State
{
    /// <summary>
    /// 初始状态
    /// </summary>
    start = 0,
    /// <summary>
    /// 绘制截图区域
    /// </summary>
    mask,
    /// <summary>
    /// 绘制工具栏
    /// </summary>
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