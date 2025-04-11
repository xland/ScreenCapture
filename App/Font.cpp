#include <QApplication>
#include "Font.h"

namespace {
    std::unique_ptr<QFont> iconFont;
}

void Font::initData(const QJsonObject& obj)
{
    QFont font("Microsoft YaHei", 9);
    qApp->setFont(font);
    auto result = QFontDatabase::addApplicationFont(":/Config/iconfont.ttf");
    if (result != 0) {
        qFatal() << "没有找到字体图标文件";
    }
    iconFont = std::make_unique<QFont>("iconfont");
    iconFont->setStyleStrategy(QFont::PreferAntialias);
    iconFont->setHintingPreference(QFont::PreferNoHinting);
}

QFont* Font::icon()
{
    return iconFont.get();
}
