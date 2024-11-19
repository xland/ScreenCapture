#pragma once
#include <Windows.h>
#include <string>
#include <memory>
#include <vector>
#include <qwidget.h>
#include <qobject.h>


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
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void initSize();
	void initBgImg();
	void initScreens();
private:
};

