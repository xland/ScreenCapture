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
	void adjustSize();
public:
	ShapeTextInput* shapeTextInput;
protected:
	void paintEvent(QPaintEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void showEvent(QShowEvent* event) override;
private:
private:
	ShapeText* shapeText;
	QPointF pressPos;
};
