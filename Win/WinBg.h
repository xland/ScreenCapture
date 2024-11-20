#pragma once
#include <Windows.h>
#include <QMouseEvent>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>

class WinBase;
class WinBg : public QWidget
{
	Q_OBJECT
public:
	WinBg(QWidget* parent = nullptr);
	~WinBg();
public:
	int x, y, w, h;
	float sf{ 1.0f };
	std::vector<QRect> screens;
	QPixmap bgImg;
	WinBase* win;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void initSize();
	void initBgImg();
	void initScreens();
private:
};

