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
protected:
	void focusOutEvent(QFocusEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
private:
private:
	bool showTextInputCursor{ true };
	ShapeText* shapeText;
};
