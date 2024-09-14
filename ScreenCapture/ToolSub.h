#pragma once

#include <QMainWindow>

class ToolSub : public QMainWindow
{
	Q_OBJECT

public:
	ToolSub(QWidget *parent = nullptr);
	~ToolSub();
	static void Init();
	static void Get();
private:
};
