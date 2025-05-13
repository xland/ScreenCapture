#pragma once

#include <QMainWindow>

class WinLongViewer : public QMainWindow
{
	Q_OBJECT

public:
	WinLongViewer(QWidget *parent = nullptr);
	~WinLongViewer();
protected:
	void paintEvent(QPaintEvent* event);
private:
	QImage img;
};
