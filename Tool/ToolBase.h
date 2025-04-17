﻿#pragma once

#include <QWidget>
#include <QPainter>

class BtnCheck;
class Btn;
class ToolBase : public QWidget
{
	Q_OBJECT
public:
	ToolBase(QWidget *parent = nullptr);
	virtual ~ToolBase();
	virtual void btnCheckChange(BtnCheck* btn);
	virtual void btnClick(Btn* btn);
public:
	int selectIndex{ -1 };
protected:
	void showEvent(QShowEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	std::shared_ptr<QPainter> getPainter();
	void paintBtn(const QChar& icon,const QColor& frontColor,const QColor& bgColor,QRect& rect, QPainter* p);
protected:
	int hoverIndex{ -1 };
	qreal btnW{ 32 };
	qreal border{ 0.8 };
private:
};
