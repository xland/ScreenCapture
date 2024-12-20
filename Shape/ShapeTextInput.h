#pragma once

#include <QObject>
#include <QTextEdit>

class ShapeText;
class ShapeTextInput  : public QTextEdit
{
	Q_OBJECT

public:
	ShapeTextInput(ShapeText* shapeText, QWidget* parent = nullptr);
	~ShapeTextInput();
public:
	bool creating{ true };
protected:
	void focusOutEvent(QFocusEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
private:
private:
	bool showTextInputCursor{ true };
	ShapeText* shapeText;
};
