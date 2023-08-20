#include "Icon.h"

static const char* icons[] = {
    reinterpret_cast<const char*>(u8"\ue8e8"), //box
    reinterpret_cast<const char*>(u8"\ue6bc"), //ellipse
    reinterpret_cast<const char*>(u8"\ue603"), //arrow
    reinterpret_cast<const char*>(u8"\ue776"), //number
    reinterpret_cast<const char*>(u8"\ue601"), //pen
    reinterpret_cast<const char*>(u8"\ue69b"), //line
    reinterpret_cast<const char*>(u8"\ue6ec"), //text
    //reinterpret_cast<const char*>(u8"\ue87d"), //image
    reinterpret_cast<const char*>(u8"\ue82e"), //mosaic
    reinterpret_cast<const char*>(u8"\ue6be"),//eraser


    reinterpret_cast<const char*>(u8"\ued85"), //undo
    reinterpret_cast<const char*>(u8"\ued8a"), //redo

    reinterpret_cast<const char*>(u8"\ue62f"),  //save
    reinterpret_cast<const char*>(u8"\ue87f"), //copy
    reinterpret_cast<const char*>(u8"\ue6e7"), //close


    reinterpret_cast<const char*>(u8"\ue602"),  //rectFill
    reinterpret_cast<const char*>(u8"\ue600"),  //ellipseFill
    reinterpret_cast<const char*>(u8"\ue604"),  //arrowFill
    reinterpret_cast<const char*>(u8"\ue605"),  //numberFill
    reinterpret_cast<const char*>(u8"\ue607"), //transparent

    reinterpret_cast<const char*>(u8"\ue608"), //dot

    reinterpret_cast<const char*>(u8"\ue61d"),  //colorUnChecked
    reinterpret_cast<const char*>(u8"\ue721"), //colorChecked
};
const char* Icon::GetIcon(Icon::Name icon)
{
    return icons[(int)icon];
}