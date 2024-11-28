#pragma once

#include <QWidget>

class PixelInfo  : public QWidget
{
	Q_OBJECT

public:
	PixelInfo(QWidget* parent=nullptr);
	~PixelInfo();
	void mouseMove(QMouseEvent* event);
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	QImage img;
};