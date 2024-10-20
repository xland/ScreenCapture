#pragma once
#include <qchar.h>
#include <qstring.h>

#include "../State.h"

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
private:

};
