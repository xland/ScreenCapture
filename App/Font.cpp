#include <QApplication>
#include "Font.h"

namespace {
    std::unique_ptr<QFont> iconFont;
}

void Font::initData(const QJsonObject& obj)
{
    QFont font("Microsoft YaHei Light", 9);
    qApp->setFont(font);
    QString fontPath = obj["fontPath"].toString();
    auto result = QFontDatabase::addApplicationFont(fontPath);
    if (result != 0) {
        qFatal() << "没有找到字体图标文件";
    }
    iconFont = std::make_unique<QFont>(obj["fontName"].toString());
    iconFont->setStyleStrategy(QFont::PreferAntialias);
    iconFont->setHintingPreference(QFont::PreferNoHinting);
}

QFont* Font::icon()
{
    return iconFont.get();
}
