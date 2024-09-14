#pragma once

#include <QWidget>
#include <qpainter.h>
#include <QMouseEvent>
#include <QPoint>

class ToolMain : public QWidget
{
	Q_OBJECT

public:
	ToolMain(QWidget *parent = nullptr);
	~ToolMain();
	static void Show(const QPoint& pos);
protected:
	void paintEvent(QPaintEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void mouseMoveEvent(QMouseEvent* event) override;
	void leaveEvent(QEvent* event) override;
private:
	qreal btnW{ 36 };
	int hoverIndex{ -1 };
	int selectIndex{ -1 };
	std::vector<QChar> iconCode{
		QChar(0xe8e8),
		QChar(0xe6bc),
		QChar(0xe603),
		QChar(0xe776),
		QChar(0xe601),
		QChar(0xe69b),
		QChar(0xe6ec),
		QChar(0xe87d),
		QChar(0xe82e),
		QChar(0xe6be),
		QChar(0xed85),
		QChar(0xed8a),
		QChar(0xe6a3),
		QChar(0xe6c0),
		QChar(0xe87f)
	};
};
