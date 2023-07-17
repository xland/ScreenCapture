#pragma once
#include <map>
class Icon
{
public:
    enum class Name
    {
        none,
        rectEllipse,
        arrow,
        line,
        mosaic,
        text,
        undo,
        redo,
        save,
        copy,
        ok,
        number,
        eraser,
        dot,
        rect,
        rectFill,
        ellipse,
        ellipseFill,
        arrowFill,
        arrowEmpty,
        bold,
        italic,

    };
    static void Init();
    static std::map<Icon::Name, const char*> icons;
};

