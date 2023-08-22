#pragma once
#include <map>
class Icon
{
public:
    enum class Name
    {
        box = 0,
        ellipse,
        arrow,
        number,
        pen,
        line,
        text,
        //image,
        mosaic,
        eraser,

        undo,
        redo,

        pin,
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

    };
    static const char* GetIcon(Icon::Name icon);
};

