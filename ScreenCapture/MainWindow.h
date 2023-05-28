#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPainter>
#include <QImage>
#include "PathModel.h"
#include "ColorSelector.h"
#include "ButtonDot.h"
#include "TextInputBox.h"
#include <QDateTime>


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
    void keyPressEvent(QKeyEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
private slots:

private:
    void initToolMain();
    void initToolRect();
    void initToolArrow();
    void initToolPen();
    void initToolMosaic();
    void initToolEraser();
    void initToolText();

    void btnMainToolSelected();
    void switchTool(const QString& toolName);
    void showToolMain();

    void moveTipBox();
    void saveToFile();
    void saveToClipboard();


    QPointF mousePressPoint;
    bool isMouseDown = false;
    bool mouseMove(QMouseEvent* mouseEvent);
    bool mousePress(QMouseEvent* mouseEvent);
    bool mouseRelease(QMouseEvent* mouseEvent);

    QPainter* layerDrawingPainter;
    QPainter* layerBgPainter;
    QPainter* layerMosaicPainter;
    QImage* layerDrawingImg;
    QImage* layerBgImg;
    QImage* layerMosaicImg;
    QVector<QImage> historyImgs;
    QVector<QPoint> historyPoints;
    bool isDrawing = false;
    void initLayer();
    void initMosaic();
    void paintPath(PathModel& path, QPainter* painter);
    PathModel& createPath();
    void endOneDraw();
    void drawArrow(QPointF& point);

    void undo();
    void redo();
    void jobLoop();

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
    void setDraggerPosition(qreal x1, qreal y1, qreal x2, qreal y2);
    void resizePath(const QPointF& point);
    bool isMouseInDragger(const QPointF& point);

    Ui::MainWindow* ui;
    int draggingTextState = 0;
    TextInputBox* textInputBox;
    ColorSelector* colorSelector;
    ButtonDot* dotRectEllipse;
    ButtonDot* dotPen;
    ButtonDot* dotMosaic;
    ButtonDot* dotEraser;
    ButtonDot* dotText;
    QString state = "Start";
    QString style = R"(#%1{background:#f6f6f6;}
#%1 QPushButton{color:#414141;font-size:16px;border:none;}
#%1 QPushButton:hover{background:#e5e5e5;}
#%1 QPushButton:disabled {color:#999;}
#%1 QPushButton:checked{background:#1677ff;color:#fff;})";
    QVector<PathModel> paths;
    qreal scaleFactor;
};

#endif // MAINWINDOW_H
