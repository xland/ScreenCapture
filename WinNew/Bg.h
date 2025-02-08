#pragma once
#include <QWidget>

namespace Win
{
	class Bg : public QWidget
	{
		Q_OBJECT

	public:
		Bg();
		~Bg();
		void fullWindow();
	protected:
		void paintEvent(QPaintEvent* event) override;
	private:
	private:
		QImage img;
	};
}
