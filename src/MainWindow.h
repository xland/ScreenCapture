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
signals:
    void shotScreenReady();
private slots:

private:
    void initWindow();
    void moveTipBox();
    void highlightWindow();
    void saveToFile();
    void saveToClipboard();

    qreal maxRate;
    QRect screenRect;
    QImage* desktopImage = nullptr;
    QList<QRect> windowRects;
    qreal scaleFactor;
    int screenSpanX[2]{-1,0};
    int screenSpanY[2]{-1,0};
    void shotScreen();
    void adjustWindowToScreen();
    void initWindowRects();
    QPoint getNativeMousePos();


    ColorSelector* colorSelector;
    ButtonDot* dotRectEllipse;
    ButtonDot* dotPen;
    ButtonDot* dotMosaic;
    ButtonDot* dotEraser;
    ButtonDot* dotText;
    int draggingTextState = 0;
    TextInputBox* textInputBox;
    QString style = R"(#%1{background:#f6f6f6;}
#%1 QPushButton{color:#414141;font-size:16px;border:none;}
#%1 QPushButton:hover{background:#e5e5e5;}
#%1 QPushButton:disabled {color:#999;}
#%1 QPushButton:checked{background:#1677ff;color:#fff;})";
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
    QList<QImage> historyImgs;
    QList<QPoint> historyPoints;
    bool isDrawing = false;
    void initLayer();
    void initMosaic();
    void paintPath(PathModel* path, QPainter* painter);
    PathModel* createPath();
    void endOneDraw();
    void drawArrow(const QPointF& startPoint,const QPointF& endPoint);

    void undo();
    void redo();
    void jobLoop();

    PathModel maskPath;
    qreal maskBorderWidth = 2.0;
    int resizeMaskIndex = -1;
    void initMask();
    void resizeMask(const QPointF& point);
    void setMask(const QPointF& leftTop, const QPointF& rightBottom);
    int pointInMaskArea(const QPointF& curPoint);

    int showDraggerCount = 0;
    int draggerIndex = -1;
    QString preState;
    QList<QRectF> dragers;
    void initDragger();
    void setRectEllipseDraggerPosition(qreal x1, qreal y1, qreal x2, qreal y2);
    void setArrowDraggerPosition(qreal x1, qreal y1, qreal x2, qreal y2);
    bool isMouseInDragger(const QPointF& point);


    QVector<PathModel*> paths;
    void editRectEllipse(const QPointF& point);
    void editArrow(const QPointF& point);
    void removeUndoPath();
    void translateTextToPath();


    Ui::MainWindow* ui;
    QString state = "Start";
    HWND hwnd;
};

#endif // MAINWINDOW_H
