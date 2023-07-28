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
        number,
        pen,
        line,
        text,
        mosaic,
        eraser,

        undo,
        redo,

        save,
        copy,
        close,
        
        rectFill,
        ellipseFill,
        arrowFill,
        numberFill,
        transparent,

        dot,

        colorUnChecked,
        colorChecked,

        bold,
        italic,

    };
    static const char* GetIcon(Icon::Name icon);
};

