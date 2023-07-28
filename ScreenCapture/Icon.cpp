#include "Icon.h"

static const char* icons[] = {
    reinterpret_cast<const char*>(u8"\ue8e8"),
    reinterpret_cast<const char*>(u8"\ue6bc"),
    reinterpret_cast<const char*>(u8"\ue603"),
    reinterpret_cast<const char*>(u8"\ue776"),
    reinterpret_cast<const char*>(u8"\ue601"),
    reinterpret_cast<const char*>(u8"\ue69b"),
    reinterpret_cast<const char*>(u8"\ue6ec"),
    reinterpret_cast<const char*>(u8"\ue82e"),
    reinterpret_cast<const char*>(u8"\ue6be"),


    reinterpret_cast<const char*>(u8"\ued85"),
    reinterpret_cast<const char*>(u8"\ued8a"),

    reinterpret_cast<const char*>(u8"\ue62f"),
    reinterpret_cast<const char*>(u8"\ue87f"),
    reinterpret_cast<const char*>(u8"\ue6e7"),


    reinterpret_cast<const char*>(u8"\ue602"),
    reinterpret_cast<const char*>(u8"\ue600"),
    reinterpret_cast<const char*>(u8"\ue604"),
    reinterpret_cast<const char*>(u8"\ue605"),
    reinterpret_cast<const char*>(u8"\ue607"),

    reinterpret_cast<const char*>(u8"\ue608"),

    reinterpret_cast<const char*>(u8"\ue61d"),
    reinterpret_cast<const char*>(u8"\ue721"),
};
const char* Icon::GetIcon(Icon::Name icon)
{
    return icons[(int)icon];
}