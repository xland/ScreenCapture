#pragma once

#include <QObject>
#include <QWidget>
#include <QMouseEvent>

class ShapeText;
class ShapeTextInput;
class ShapeTextContainer  : public QWidget
{
	Q_OBJECT

public:
	ShapeTextContainer(ShapeText* shapeText, QWidget* parent = nullptr);
	~ShapeTextContainer();
public:
	ShapeTextInput* shapeTextInput;
	bool isPress{ false };
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
private:
	void adjustSize();
private:
	ShapeText* shapeText;
	QPointF pressPos;
};
