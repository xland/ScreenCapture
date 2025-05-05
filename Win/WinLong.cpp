#include <Windows.h>
#include "WinLong.h"

WinLong::WinLong(QWidget *parent) : WinBase(parent)
{

	//UINT scrollLines;
	//SystemParametersInfo(SPI_GETWHEELSCROLLLINES, 0, &scrollLines, 0);
	//int lines = delta * scrollLines / WHEEL_DELTA; // 计算滚动行数


	//SCROLLINFO si = { sizeof(SCROLLINFO) };
	//si.fMask = SIF_POS;
	//GetScrollInfo(hWnd, SB_VERT, &si);
	//int oldPos = si.nPos; // 记录滚动前位置

	//SetScrollInfo(hWnd, SB_VERT, &si, TRUE); // 更新滚动位置
	//GetScrollInfo(hWnd, SB_VERT, &si); // 获取新位置
	//int scrollDistance = si.nPos - oldPos; // 计算滚动距离（单位）


	//HDC hdc = GetDC(hWnd);
	//TEXTMETRIC tm;
	//GetTextMetrics(hdc, &tm);
	//int lineHeight = tm.tmHeight; // 每行高度（像素）
	//ReleaseDC(hWnd, hdc);
}

WinLong::~WinLong()
{

}

void WinLong::paintEvent(QPaintEvent* event)
{
}

void WinLong::mousePressEvent(QMouseEvent* event)
{
}

void WinLong::mouseMoveEvent(QMouseEvent* event)
{
}

void WinLong::mouseReleaseEvent(QMouseEvent* event)
{
}

void WinLong::closeEvent(QCloseEvent* event)
{
}

QImage WinLong::getTargetImg()
{
	return QImage();
}

void WinLong::initWindow()
{
}
