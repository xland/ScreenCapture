#pragma once
#include <QWidget>



namespace Win
{
	class Box;
	class Bg : public QWidget
	{
		Q_OBJECT

	public:
		Bg(Box* box);
		~Bg();
		void fullWindow();
		QImage grabImg(const int& x, const int& y, const int& w, const int& h);
	protected:
		void paintEvent(QPaintEvent* event) override;
	private:
	private:
		QImage img;
		Box* box;
	};
}
