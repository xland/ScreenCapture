#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include "PathModel.h"


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow

{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
public:
protected:
    bool eventFilter(QObject* obj, QEvent* event) override;
    void paintEvent(QPaintEvent*) override;
private slots:

private:
    void initToolMain();
    void initToolRect();
    void initToolArrow();
    void initToolPen();
    void initToolMosaic();
    void initToolEraser();
    void btnMainToolSelected();
    void switchTool(const QString& toolName);
    void showToolMain();


    QPointF mousePressPoint;
    bool isMouseDown = false;
    bool mouseMove(QMouseEvent* mouseEvent);
    bool mousePress(QMouseEvent* mouseEvent);
    bool mouseRelease(QMouseEvent* mouseEvent);

    QPainter* painter1;
    QPainter* painter2;
    QImage* canvasImg1;
    QImage* canvasImg2;
    QImage* mosaicImg;
    void initCanvasImg();
    void initMosaicImg();
    void paintPath(PathModel& path, QPainter* painter);
    void paintMosaic(PathModel& path, QPainter* painter, qreal x1, qreal y1, qreal x2, qreal y2);

    void undo();
    void redo();

    PathModel maskPath;
    qreal maskBorderWidth = 2.0;
    int resizeMaskIndex = -1;
    void initMask();
    void resizeMask(const QPointF& point);
    int pointInMaskArea(const QPointF& curPoint);

    int showDraggerCount = 0;
    int draggerIndex = -1;
    QString preState;
    QVector<QRectF> dragers;
    void initDragger();
    void draggerReleased();
    void setDraggerPosition(qreal x1, qreal y1, qreal x2, qreal y2);
    void resizePath(const QPointF& point);
    bool isMouseInDragger(const QPointF& point);

    Ui::MainWindow* ui;
    QString state = "Start";
    QString style = R"(#%1{background:#f6f6f6;}
#%1 QPushButton{color:#414141;font-size:16px;border:none;}
#%1 QPushButton:hover{background:#e5e5e5;}
#%1 QPushButton:disabled {color:#999;}
#%1 QPushButton:checked{background:#1677ff;color:#fff;})";
    QVector<PathModel> paths;
};

#endif // MAINWINDOW_H
