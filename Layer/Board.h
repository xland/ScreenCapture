#pragma once

#include <QWidget>
#include <QTimer>

class ShapeBase;
class Board : public QWidget
{
	Q_OBJECT
public:
	Board(QWidget *parent);
	~Board();
protected:
	void paintEvent(QPaintEvent* event) override;
private:
private:
};
