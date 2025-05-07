#pragma once

#include <QMainWindow>

class WinLongTip : public QMainWindow
{
	Q_OBJECT

public:
	WinLongTip(QWidget *parent = nullptr);
	~WinLongTip();
	void mouseMove(const QPoint& nativePos);
protected:
	void paintEvent(QPaintEvent* event) override;
	void closeEvent(QCloseEvent* event) override;
private:
};
