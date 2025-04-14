#pragma once

#include <QWidget>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "ToolBtn.h"

class WinBox;
class BtnBase;
class BtnCheck;
class ToolBase : public QWidget
{
	Q_OBJECT
public:
	ToolBase(WinBox* win,QWidget *parent = nullptr);
	virtual ~ToolBase();
	virtual void btnCheckChange(BtnCheck* btn);
public:
	int selectIndex{ -1 };
	WinBox* win;
protected:
	void enterEvent(QEnterEvent* event) override;
	void leaveEvent(QEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;
	std::shared_ptr<QPainter> getPainter();
	void paintBtn(const QChar& icon,const QColor& frontColor,const QColor& bgColor,QRect& rect, QPainter* p);
protected:
	int hoverIndex{ -1 };
	qreal btnW{ 32 };
	qreal border{ 0.8 };
private:
};
