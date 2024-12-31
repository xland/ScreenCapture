#include <QGuiApplication>
#include "About.h"


About::About(QWidget* parent):QWidget(parent)
{
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover);
    setAttribute(Qt::WA_QuitOnClose, false);
    QTextEdit* textEdit = new QTextEdit(this);
    QString htmlText = R"(<html><head><style>
body { font-family: Arial; font-size: 16px; color: #333;text-align:center;}
h1 { color: #007BFF;line-height: 2.0; }
p { line-height: 1.5; }
</style></head><body>
<h1>ScreenCapture</h1>
<p><a href="https://github.com/xland/ScreenCapture">https://github.com/xland/ScreenCapture</a></p>
</body></html>)";
    textEdit->setHtml(htmlText);
    textEdit->setReadOnly(true);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(textEdit);
    layout->setContentsMargins(12, 12, 12, 12);
    setLayout(layout);
    setFixedSize(500, 160);
    show();
}

void About::enterEvent(QEnterEvent* event)
{
    QGuiApplication::setOverrideCursor(Qt::ArrowCursor);
}

void About::leaveEvent(QEvent* event)
{
}
