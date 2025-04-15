#pragma once

#include <QWidget>

class PixelInfo  : public QWidget
{
	Q_OBJECT

public:
	PixelInfo(QWidget* parent=nullptr);
	~PixelInfo();
	void mouseMove(const QPoint& nativePos);
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
private:
	QPoint nativePos;
};
