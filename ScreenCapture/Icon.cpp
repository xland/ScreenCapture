#include "Icon.h"

std::map<Icon::Name, const char*> Icon::icons;
void Icon::Init()
{
    icons.insert({ Name::rectEllipse, reinterpret_cast<const char*>(u8"\ue86d") });
    icons.insert({ Name::arrow, reinterpret_cast<const char*>(u8"\ueb0c") });
    icons.insert({ Name::line, reinterpret_cast<const char*>(u8"\ue601") });
    icons.insert({ Name::mosaic, reinterpret_cast<const char*>(u8"\ue82e") });
    icons.insert({ Name::text, reinterpret_cast<const char*>(u8"\ue6ec") });
    icons.insert({ Name::undo, reinterpret_cast<const char*>(u8"\ued85") });
    icons.insert({ Name::redo, reinterpret_cast<const char*>(u8"\ued8a") });
    icons.insert({ Name::save, reinterpret_cast<const char*>(u8"\ue62f") });
    icons.insert({ Name::copy, reinterpret_cast<const char*>(u8"\ue87f") });
    icons.insert({ Name::ok, reinterpret_cast<const char*>(u8"\ue633") });
    icons.insert({ Name::number, reinterpret_cast<const char*>(u8"\ue776") });
    icons.insert({ Name::eraser, reinterpret_cast<const char*>(u8"\ue6b8") });
    icons.insert({ Name::dot, reinterpret_cast<const char*>(u8"\ue608") });
    icons.insert({ Name::rectFill, reinterpret_cast<const char*>(u8"\ueaa2") });
    icons.insert({ Name::rect, reinterpret_cast<const char*>(u8"\ue790") });
    icons.insert({ Name::ellipse, reinterpret_cast<const char*>(u8"\ue610") });
    icons.insert({ Name::ellipseFill, reinterpret_cast<const char*>(u8"\ue6bb") });
    icons.insert({ Name::arrowFill, reinterpret_cast<const char*>(u8"\ue62c") });
    icons.insert({ Name::arrowEmpty, reinterpret_cast<const char*>(u8"\ue61a") });
    icons.insert({ Name::bold, reinterpret_cast<const char*>(u8"\ue7c2") });
    icons.insert({ Name::italic, reinterpret_cast<const char*>(u8"\ue61c") });
}