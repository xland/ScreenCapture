- 主工具栏
	- 要保持在屏幕内，动态调整位置
	- 拖动MaskRect后，好像主工具条会显示不出来（偶发）
- 子工具栏
	- 文本字体选择
	- 马赛克、橡皮擦子工具栏上面的三角
- 绘制箭头
	- 按住Shift键，水平，45度，垂直。
- 绘制曲线
	- 按住Shift键，水平，45度，垂直。
- 颜色选择器
	- 选不中颜色的问题还是偶尔会出现 
- CutMask
	- 移动时不允许超出屏幕区域
- 选中一个控件，然后删除这个控件
- 文本输入框，focusOut后，会闪一下
- CutMast刚渲染时会闪一下
- 矩形橡皮擦
- 橡皮擦矩形和线条粗细是互斥的
- 橡皮擦选中矩形后，应该隐藏stroke
- 得搞一套日志系统
- 截图区域大小位置







```
#include <QApplication>
#include <QBackingStore>
#include <QWindow>
#include <QPainter>
#include <QRect>
#include <QDebug>
#include <windows.h>

void drawContent(QBackingStore *backingStore, QWindow *window) {
    QRect rect = window->geometry();
    backingStore->beginPaint(rect);

    QPainter painter(backingStore->paintDevice());
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect, Qt::white);
    painter.setBrush(Qt::green);
    painter.drawEllipse(100, 100, 200, 200);
    painter.end();

    backingStore->endPaint();
    backingStore->flush(rect);
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    HWND hwnd = /* 原生窗口句柄 */;
    if (!hwnd) {
        qDebug() << "Failed to get native window handle!";
        return -1;
    }

    QWindow *nativeWindow = QWindow::fromWinId(reinterpret_cast<WId>(hwnd));
    QBackingStore *backingStore = new QBackingStore(nativeWindow);

    nativeWindow->setGeometry(0, 0, 800, 600);
    backingStore->resize(nativeWindow->size());

    QObject::connect(nativeWindow, &QWindow::exposed, [&]() {
        if (nativeWindow->isExposed()) {
            drawContent(backingStore, nativeWindow);
        }
    });

    nativeWindow->requestUpdate();
    nativeWindow->show();

    int result = app.exec();

    delete backingStore;
    delete nativeWindow;

    return result;
}
```