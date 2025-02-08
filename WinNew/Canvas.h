#pragma once

#include <Windows.h>
#include <QOpenGLWindow>
#include <QImage>
#include <QOpenGLPaintDevice>
#include <QOpenGLTexture>
#include <QOpenGLFunctions>

namespace Win
{
	class Canvas : public QOpenGLWindow, protected QOpenGLFunctions
	{
		Q_OBJECT

	public:
		Canvas();
		~Canvas();
		void fullWindow();
	protected:
		void initializeGL() override;
		void paintGL() override;
		void mousePressEvent(QMouseEvent* event) override;
		void mouseReleaseEvent(QMouseEvent* event) override;
		void mouseMoveEvent(QMouseEvent* event) override;
		void closeEvent(QCloseEvent* event) override;
	private:
		bool isPressed = false;
		QPoint posPress;
		QRect rectMask;
	};
}
