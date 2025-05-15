#pragma once

#include <QMainWindow>
#include <QPushButton>

class ToolLong;
class WinLongViewer : public QMainWindow
{
	Q_OBJECT

public:
	WinLongViewer(const int& areaX, const int& areaY, const int& areaW, const int& areaH, QWidget *parent = nullptr);
	~WinLongViewer();
	void saveToClipboard();
	void saveToFile();
	void pin();
protected:
	void paintEvent(QPaintEvent* event);
	void closeEvent(QCloseEvent* event) override;
private:
	void adjustPosSize();
	void timerFunc();
	void capStep();
private:
	QImage img1;
	QImage imgResult;
	QImage imgSmall;
	HWND targetHwnd;
	QTimer* stepTimer;
	int areaX, areaY, areaW, areaH;
	int topIgnoreSpan{0};
	bool firstCheck{ true };
	ToolLong* tools;
};
