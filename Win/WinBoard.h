#pragma once

#include <QWidget>


class WinBase;
class WinBoard : public QWidget
{
	Q_OBJECT
public:
	WinBoard(QWidget *parent = nullptr);
	~WinBoard();
	void init(WinBase* winBase);
	void refresh();
public:
	WinBase* winBase;
protected:
	void paintEvent(QPaintEvent* event) override;
	void initWindow();
private:
	bool refreshFlag{ false };
};
