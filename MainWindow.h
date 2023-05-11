
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainterPath>
#include <QPainter>
#include <QImage>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

enum class State
{
    start,
    rect,
    rectFill,
    ellipse,
    ellipseFill
};

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    void initToolMain();
    void initToolRect();
    void showToolMain(int x, int y);
    void showToolSub(int x, int y);

    void hideTool();
protected:
    void mouseMoveEvent(QMouseEvent* mouseEvent) override;
    void mousePressEvent(QMouseEvent* mouseEvent) override;
    void mouseReleaseEvent(QMouseEvent* mouseEvent) override;
    void paintEvent(QPaintEvent*) override;
private slots:

private:
    void btnMainToolSelected();
    void initMask();
    Ui::MainWindow* ui;
    QString style = R"(#%1{background:#f6f6f6;}
#%1 QPushButton{color:#414141;font-size:16px;border:none;}
#%1 QPushButton:hover{background:#e5e5e5;}
#%1 QPushButton:checked{background:#1677ff;color:#fff;})";
    QPointF mousePressPoint;
    bool isMouseDown = false;
    QPainterPath maskPath;
    qreal maskBorderWidth = 2.0;
    QPainter* painter;
    QImage* canvasImg;
    State state = State::start;
    QVector<QPainterPath> paths;
};

#endif // MAINWINDOW_H
