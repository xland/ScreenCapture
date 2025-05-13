#pragma once

#include <QMainWindow>

class WinLongViewer : public QMainWindow
{
	Q_OBJECT

public:
	WinLongViewer(const int& areaX, const int& areaY, const int& areaW, const int& areaH, QWidget *parent = nullptr);
	~WinLongViewer();
	void capStep();
	void capOneStep();
protected:
	void paintEvent(QPaintEvent* event);
	void closeEvent(QCloseEvent* event) override;
private:
	void adjustPosSize();
private:
	QImage img;
	int scaleNum{ 2 };
	HWND targetHwnd;
	QTimer* stepTimer;
	int areaX, areaY, areaW, areaH;
	int topIgnoreSpan{0};
};
