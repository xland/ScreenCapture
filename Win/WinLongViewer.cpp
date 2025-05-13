#include <QPainter>
#include "WinLong.h"
#include "CutMask.h"
#include "App/Util.h"
#include "WinLongViewer.h"

WinLongViewer::WinLongViewer(QWidget *parent) : QMainWindow(parent)
{
	auto win = (WinLong*)parent;
	auto dpr = parent->devicePixelRatio();
	auto tl = win->cutMask->rectMask.topLeft();
	auto br = win->cutMask->rectMask.bottomRight();
	tl = Util::getScreenPos(mapToGlobal(tl));
	br = Util::getScreenPos(mapToGlobal(br));
	img = Util::printScreen(tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y());
	img.setDevicePixelRatio(dpr);

	setFixedSize(img.width()/6, img.height()/6 + 36);
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);

	move(win->cutMask->rectMask.bottomRight() - QPoint(-3*dpr, height() - 3 * dpr));
    show();
}

WinLongViewer::~WinLongViewer()
{}

void WinLongViewer::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
	auto r = rect().adjusted(0, 0, 0, -36);
	QImage scaledImg = img.scaled(img.size()/6*devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    p.drawImage(0,0, scaledImg);
}