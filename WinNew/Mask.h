#pragma once

#include <QObject>
#include <QImage>
#include <QPainter>
#include <QPaintEvent>

namespace Win {
	class Box;
	class Mask : public QObject
	{
		Q_OBJECT
	public:
		Mask(QObject* parent);
		~Mask();
		void mousePress(QMouseEvent* event);
		void mouseDrag(QMouseEvent* event);
		void mouseRelease(QMouseEvent* event);
		void mouseMove(QMouseEvent* event);
		void paint(QPainter& p);
	public:
		QRect maskRect;
	protected:
	private:
		void changeMaskRect(const QPoint& pos);
		void changeMousePosState(const int& x, const int& y);
		void changeMousePosState2(const int& x, const int& y);

		void paintMask(QPainter& p);
		void paintMaskRectInfo(QPainter& p);
		void paintMaskRectBorder(QPainter& p);
		void moveMaskRect(const QPoint& pos);
	private:
		Box* box;
		QPoint posPress;
		qreal maskStroke{ 2.0 };
		int mousePosState{ -1 };
	};
}
