#pragma once

#include <QWidget>

class WinBase;
class WinBaseLayer : public QWidget
{
	Q_OBJECT

public:
	WinBaseLayer(QWidget* parent = nullptr);
	~WinBaseLayer();
	void init(WinBase* father);
public:
	WinBase* father;
protected:
	void showEvent(QShowEvent* event) override;
	void initWindow();
};
