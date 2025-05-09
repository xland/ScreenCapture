#include <Windows.h>
#include <QTimer>
#include "WinLong.h"
#include "CutMask.h"
#include "Canvas.h"
#include "App/Util.h"
#include "WinLongTip.h"
//#include "opencv2/opencv.hpp"

WinLong::WinLong(QWidget *parent) : WinBase(parent)
{
	x = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y = GetSystemMetrics(SM_YVIRTUALSCREEN);
	w = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	h = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	cutMask = new CutMask(this);
	imgBg = Util::printScreen(x, y, w, h);
	imgBg.setDevicePixelRatio(devicePixelRatio());
	initWindow();

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
	QPainter p(this);
	p.drawImage(0,0, imgBg);
	cutMask->paint(p);
}

void WinLong::mousePressEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		if (state < State::mask) {
			cutMask->mousePress(event);
		}
		else if (state <= State::tool) {
			cutMask->mousePress(event);
		}
	}
	else {
		if (state < State::mask) {
			qApp->exit(2);
		}
	}
}

void WinLong::mouseMoveEvent(QMouseEvent* event)
{
	if (event->buttons() & Qt::LeftButton) {
		if (state <= State::tool) {
			cutMask->mouseDrag(event);
		}
	}
	else {
		if (winLongTip) {
			winLongTip->mouseMove(event->pos());
		}
		if (state <= State::tool) {
			cutMask->mouseMove(event);
		}
	}
}

//bool findMatchingRegionOptimized(const QImage& image1, const QImage& image2, int& y1, int& y2, int& y3) {
//	auto mat1 = cv::Mat(image1.height(), image1.width(), CV_8UC4, (void*)image1.bits(), image1.bytesPerLine());
//	auto mat2 = cv::Mat(image2.height(), image2.width(), CV_8UC4, (void*)image2.bits(), image2.bytesPerLine());
//	// 转换为灰度以加速匹配
//	cv::cvtColor(mat1, mat1, cv::COLOR_BGRA2GRAY);
//	cv::cvtColor(mat2, mat2, cv::COLOR_BGRA2GRAY);
//	int height = mat1.rows;
//	int width = mat1.cols;
//	// 缩小图像进行粗略匹配
//	cv::Mat mat1Small, mat2Small;
//	cv::resize(mat1, mat1Small, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
//	cv::resize(mat2, mat2Small, cv::Size(), 0.5, 0.5, cv::INTER_LINEAR);
//
//	double bestScore = -1;
//	int bestY1 = 0, bestY2 = 0;
//
//	// 粗略匹配
//#pragma omp parallel
//	{
//		double localBestScore = -1;
//		int localBestY1 = 0, localBestY2 = 0;
//
//#pragma omp for nowait
//		for (int y1Small = 0; y1Small < mat1Small.rows; ++y1Small) {
//			int regionHeightSmall = mat1Small.rows - y1Small;
//			if (regionHeightSmall < 20) continue; // 跳过过小的区域
//
//			cv::Mat templateImg = mat2Small(cv::Rect(0, y1Small, mat1Small.cols, regionHeightSmall));
//			cv::Mat result;
//			cv::matchTemplate(mat1Small, templateImg, result, cv::TM_CCOEFF_NORMED);
//
//			double maxVal;
//			cv::Point maxLoc;
//			cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);
//
//			if (maxVal > localBestScore && maxVal > 0.9) {
//				localBestScore = maxVal;
//				localBestY1 = maxLoc.y;
//				localBestY2 = y1Small;
//			}
//		}
//#pragma omp critical
//		{
//			if (localBestScore > bestScore) {
//				bestScore = localBestScore;
//				bestY1 = localBestY1;
//				bestY2 = localBestY2;
//			}
//		}
//	}
//
//	// 验证最佳候选区域
//	if (bestScore > 0.8) {
//		int regionHeight = height - bestY1 * 2;
//		if (bestY2 * 2 + regionHeight <= height) {
//			cv::Mat templateImg = mat2(cv::Rect(0, bestY2 * 2, width, regionHeight));
//			cv::Mat result;
//			cv::matchTemplate(mat1, templateImg, result, cv::TM_CCOEFF_NORMED);
//
//			double maxVal;
//			cv::Point maxLoc;
//			cv::minMaxLoc(result, nullptr, &maxVal, nullptr, &maxLoc);
//
//			if (maxVal > 0.85) { // 更严格的阈值
//				y1 = maxLoc.y;
//				y2 = bestY2 * 2;
//				y3 = y2 + regionHeight;
//				return true;
//			}
//		}
//	}
//
//	std::cerr << "No matching region found!" << std::endl;
//	return false;
//}

void WinLong::mouseReleaseEvent(QMouseEvent* event)
{
	if (state < State::capLong) {
		state = State::capLong;
		setCursor(Qt::BlankCursor);
		winLongTip = new WinLongTip(this);
		winLongTip->mouseMove(QCursor::pos());
		winLongTip->show();
	}
	else {	
		QImage img1("D:\\project\\ScreenCapture\\Doc\\11.png");
		QImage img2("D:\\project\\ScreenCapture\\Doc\\22.png");

		//auto img11 = img1.copy(0, 180, 1570, 820);
		//auto img22 = img2.copy(0, 180, 1570, 820);
		//img11.save("Doc\\11.png");
		//img22.save("Doc\\22.png");
		int y1, y2, y3;
		//findMatchingRegionOptimized(img1, img2, y1, y2, y3);
		// 
		// 
		// hide();
		// QTimer* timer = new QTimer(this);
		// connect(timer, &QTimer::timeout, []() {
		// 	INPUT input = { 0 };
		// 	input.type = INPUT_MOUSE;
		// 	input.mi.dwFlags = MOUSEEVENTF_WHEEL;
		// 	input.mi.mouseData = -WHEEL_DELTA;
		// 	SendInput(1, &input, sizeof(INPUT));
		// 	qDebug() << "send";
		// });
		// timer->start(600);		
	}
}

void WinLong::closeEvent(QCloseEvent* event)
{
	deleteLater();
	qApp->exit(1);
}

QImage WinLong::getTargetImg()
{
	return QImage();
}

void WinLong::initWindow()
{
	setAutoFillBackground(false);
	setAttribute(Qt::WA_OpaquePaintEvent);
	setAttribute(Qt::WA_NoSystemBackground);
	setAttribute(Qt::WA_TranslucentBackground, true);
	setGeometry(x, y, w, h);
#ifdef DEBUG
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
#else
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::WindowStaysOnTopHint);
#endif    
	show();
	auto hwnd = (HWND)winId();
	SetWindowPos(hwnd, nullptr, x, y, w, h, SWP_NOZORDER | SWP_SHOWWINDOW);
	setMouseTracking(true);
	setCursor(Qt::CrossCursor);
}

void WinLong::joinImg(const QImage& img11, const QImage& img22) {
	if (resultImg.isNull()) {
		resultImg = img11;
	}
	auto w = img11.width();
	auto h = img11.height();
	auto img1 = img11.convertToFormat(QImage::Format_Grayscale8);
	auto img2 = img22.convertToFormat(QImage::Format_Grayscale8);
	int diffY = -1; //从此处开始不一样
	int sameY = -1;
	for (size_t y = 0; y < h; y++)
	{
		bool flag{ true }; //判断是否一行像素都相同
		for (size_t x = 0; x < 300; x++)
		{
			if (diffY == -1) {
				auto pix1 = img1.pixel(x, y);
				auto pix2 = img2.pixel(x, y);
				if (pix1 != pix2) {
					diffY = y;
					flag = false;//为了继续执行比较
					break;
				}
			}
			else {
				if (y >= 616) {
					int x = 0;
				}
				auto pix1 = img1.pixel(x, y);
				auto pix2 = img2.pixel(x, diffY);
				if (pix1 != pix2) {
					flag = false;
					break;
				}
			}
		}
		if (diffY != -1 && flag == true) {
			sameY = y;
			break;
		}
	}
	if (diffY == -1) return;
	QImage result(w, resultImg.height() + (h - diffY), QImage::Format_ARGB32);
	QPainter p(&result);
	auto img = resultImg.copy(0, 0, w, resultImg.height() - (h-sameY));
	p.drawImage(0, 0, img);

	result.save("333.png");


	//img = img2.copy(0, diffY, w, h - diffY);
	//p.drawImage(0, resultImg.height()- (h - sameY), img);
	//p.end();
	//resultImg = result;
}