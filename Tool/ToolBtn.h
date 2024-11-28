#pragma once
#include <QChar>
#include <QString>

#include "../App/State.h"

class ToolBtn
{
public:
	ToolBtn() = default;
	~ToolBtn() = default;
public:
	bool selected;
	QChar icon;
	QString tipText;
	State state;
	QString name;
	unsigned min;
	unsigned max;
	unsigned value;
	unsigned selectedIndex;
private:

};
