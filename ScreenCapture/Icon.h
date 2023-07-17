#pragma once
#include <map>
class Icon
{
public:
    enum class Name
    {
        rect = 0,
        ellipse,
        arrow,
        pen,
        line,
        mosaic,
        text,
        number,
        eraser,

        undo,
        redo,

        save,
        copy,
        close,

        dot,
        rectFill,
        ellipseFill,
        arrowFill,
        arrowEmpty,
        bold,
        italic,

    };
    static const char* Get(Icon::Name icon);
};

