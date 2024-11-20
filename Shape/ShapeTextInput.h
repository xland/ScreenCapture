#pragma once

#include <QObject>
#include <qtextedit.h>

class ShapeTextInput  : public QTextEdit
{
	Q_OBJECT

public:
	ShapeTextInput(QWidget* parent = nullptr);
	~ShapeTextInput();
signals:
	void focusOut();
	void focusIn();
public:
	QColor textInputCursorColor;
private:
	void focusOutEvent(QFocusEvent* event) override;
	void focusInEvent(QFocusEvent* event) override;
	void paintEvent(QPaintEvent* event) override;
private:
	bool showTextInputCursor{ true };
};
