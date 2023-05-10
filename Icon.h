
#ifndef ICON_H
#define ICON_H

#include <QFont>
#include <QMap>
#include <QFontDatabase>



class Icon
{
public:
    enum class Name
    {
        none,
        rect,
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
        eraser
    };
    static void Init();
    static QFont font;
    static QMap<Icon::Name, QChar> icons;
private:

};

#endif // ICON_H
