#pragma once

#include <QObject>
#include <QWidget>

class ShapeText;
class ShapeTextInput;
class ShapeTextContainer  : public QWidget
{
	Q_OBJECT

public:
	ShapeTextContainer(ShapeText* shapeText, QWidget* parent = nullptr);
	~ShapeTextContainer();
	QRect getNativeRect();
public:
	bool creating{ true };
	bool painting{ false };
protected:
	void paintEvent(QPaintEvent* event) override;
private:
	void adjustSize();
private:
	ShapeText* shapeText;
	ShapeTextInput* shapeTextInput;
};
