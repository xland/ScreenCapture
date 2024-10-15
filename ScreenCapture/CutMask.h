#pragma once

#include <QWidget>

class CutMask  : public QWidget
{
	Q_OBJECT

public:
	CutMask(QWidget *parent = nullptr);
	~CutMask();
	static void Init();
	static CutMask* Get();
};
