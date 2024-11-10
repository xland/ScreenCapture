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
private:
	void focusOutEvent(QFocusEvent* event) override;
	//void paintEvent(QPaintEvent*) override;
};
