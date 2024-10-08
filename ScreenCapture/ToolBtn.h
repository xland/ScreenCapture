#pragma once
#include <qchar.h>
#include <qstring.h>

#include "BtnType.h"

class ToolBtn
{
public:
	ToolBtn() = default;
	~ToolBtn() = default;
public:
	bool selected;
	QChar icon;
	QString name;
	QString zhcn;
	QString en;
private:

};