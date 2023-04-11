
#ifndef CUTTER_H
#define CUTTER_H


#include <QObject>
#include "ScreenShoter.h"

class Cutter : public QObject
{
    Q_OBJECT
    Q_PROPERTY (int absoluteX READ getAbsoluteX CONSTANT)
    Q_PROPERTY (int absoluteY READ getAbsoluteY CONSTANT)
    Q_PROPERTY (int totalWidth READ getTotalWidth CONSTANT)
    Q_PROPERTY (int totalHeight READ getTotalHeight CONSTANT)

    Q_PROPERTY (int cutAreaLeft READ getCutAreaLeft WRITE setCutAreaLeft NOTIFY cutAreaLeftChanged)
    Q_PROPERTY (int cutAreaTop READ getCutAreaTop WRITE setCutAreaTop NOTIFY cutAreaTopChanged)
    Q_PROPERTY (int cutAreaRight READ getCutAreaRight WRITE setCutAreaBottom NOTIFY cutAreaRightChanged)
    Q_PROPERTY (int cutAreaBottom READ getCutAreaBottom WRITE setCutAreaBottom NOTIFY cutAreaBottomChanged)


    Q_PROPERTY (int mouseX READ getMouseX WRITE setMouseX NOTIFY mouseXChanged)
    Q_PROPERTY (int mouseY READ getMouseY WRITE setMouseY NOTIFY mouseYChanged)
    Q_PROPERTY (QString colorRgb READ getColorRgb WRITE setColorRgb NOTIFY colorRgbChanged)
    Q_PROPERTY (QString colorHex READ getColorHex WRITE setColorHex NOTIFY colorHexChanged)

public:
    Q_INVOKABLE void getHoveredWindowRect();
    ~Cutter();
    static void Init(QObject *parent);
    static Cutter* Get();
    void setMouseX(int val);
    void setMouseY(int val);
    void setColorRgb(QString val);
    void setColorHex(QString val);
signals:
    void cutAreaLeftChanged(int val);
    void cutAreaTopChanged(int val);
    void cutAreaRightChanged(int val);
    void cutAreaBottomChanged(int val);
    void mouseXChanged(int val);
    void mouseYChanged(int val);
    void colorRgbChanged(QString val);
    void colorHexChanged(QString val);
private:
    explicit Cutter(QObject *parent = nullptr);
    int getAbsoluteX() const { return ScreenShoter::Get()->absoluteX; }
    int getAbsoluteY() const { return ScreenShoter::Get()->absoluteY; }
    int getTotalWidth() const { return ScreenShoter::Get()->totalWidth; }
    int getTotalHeight() const { return ScreenShoter::Get()->totalHeight; }

    int getCutAreaLeft(){return cutAreaLeft;}
    int getCutAreaTop(){return cutAreaTop;}
    int getCutAreaRight(){return cutAreaRight;}
    int getCutAreaBottom(){return cutAreaBottom;}
    int getMouseX(){return mouseX;}
    int getMouseY(){return mouseY;}
    QString getColorRgb(){return colorRgb;}
    QString getColorHex(){return colorHex;}

    void setCutAreaLeft(int val);
    void setCutAreaTop(int val);
    void setCutAreaRight(int val);
    void setCutAreaBottom(int val);

    int cutAreaLeft = 0;
    int cutAreaTop = 0;
    int cutAreaRight = 0;
    int cutAreaBottom = 0;
    int mouseX = 0;
    int mouseY = 0;
    QString colorRgb;
    QString colorHex;

signals:

};

#endif // CUTTER_H
