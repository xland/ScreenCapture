#include "Icon.h"

static const char* icons[] = {
    reinterpret_cast<const char*>(u8"\ue790"),
    reinterpret_cast<const char*>(u8"\ue791"),
    reinterpret_cast<const char*>(u8"\ueb0c"),
    reinterpret_cast<const char*>(u8"\ue601"),
    reinterpret_cast<const char*>(u8"\ue69b"),
    reinterpret_cast<const char*>(u8"\ue82e"),
    reinterpret_cast<const char*>(u8"\ue6ec"),
    reinterpret_cast<const char*>(u8"\ue776"),
    reinterpret_cast<const char*>(u8"\ue6b8"),


    reinterpret_cast<const char*>(u8"\ued85"),
    reinterpret_cast<const char*>(u8"\ued8a"),

    reinterpret_cast<const char*>(u8"\ue62f"),
    reinterpret_cast<const char*>(u8"\ue87f"),
    reinterpret_cast<const char*>(u8"\ue6e7"),
};
const char* Icon::Get(Icon::Name icon)
{
    return icons[(int)icon];
}