#include <QPainter>
#include <QtConcurrent>
#include <vector>
#include <limits>
#include <tuple>
#include <cmath>
#include "WinLong.h"
#include "CutMask.h"
#include "App/Util.h"
#include "WinLongViewer.h"

WinLongViewer::WinLongViewer(const int& areaX, const int& areaY, const int& areaW, const int& areaH, QWidget *parent) : QMainWindow(parent),
areaX{ areaX }, areaY{ areaY }, areaW{ areaW }, areaH{ areaH }
{

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    img = Util::printScreen(areaX, areaY, areaW, areaH);
    img.setDevicePixelRatio(devicePixelRatio());
    adjustPosSize();

    show();

    POINT pt;
    GetCursorPos(&pt);
    targetHwnd = WindowFromPoint(pt);
    stepTimer = new QTimer(this);
    connect(stepTimer, &QTimer::timeout, this, &WinLongViewer::capOneStep);
    stepTimer->start(800);
    SetCursorPos(pt.x + 1, pt.y + 1); //开始滚动
}

WinLongViewer::~WinLongViewer()
{}

#pragma region imgJoinMethod

struct MatchResult {
    int y;
    double error;
};

MatchResult computeSSDAtY(int y, const QImage& gray1, const QImage& gray2) {
    const int width = gray1.width();
    const int blockHeight = gray2.height();
    double error = 0.0;
    const uchar* p1 = gray1.constBits() + y * gray1.bytesPerLine();
    const uchar* p2 = gray2.constBits();
    for (int row = 0; row < blockHeight; ++row) {
        const uchar* row1 = p1 + row * gray1.bytesPerLine();
        const uchar* row2 = p2 + row * gray2.bytesPerLine();
        for (int x = 0; x < width; ++x) {
            int diff = int(row1[x]) - int(row2[x]);
            error += diff * diff;
        }
    }
    return { y, error };
}

int findMostSimilarRegionParallel(const QImage& gray1, const QImage& gray2) {
    const int searchHeight = gray1.height() - gray2.height() + 1;
    // 构造搜索区间
    QList<int> yList;
    for (int y = 0; y < searchHeight; ++y) {
        yList.append(y);
    }
    // 并行计算每个 y 的误差
    auto results = QtConcurrent::blockingMapped(yList,[&](int y) { return computeSSDAtY(y, gray1, gray2); });
    // 取最小误差的结果
    MatchResult best = *std::min_element(results.begin(), results.end(),[](const MatchResult& a, const MatchResult& b) {
            return a.error < b.error;
        });
    return best.y;
}

size_t findTopSameHeight(const QImage& img1, const QImage& img2) {

    for (size_t y = 0; y < img1.height(); y++)
    {
        bool same;
        for (size_t x = 0; x < img1.width(); x++)
        {
            if (img1.pixel(x, y) != img2.pixel(x, y)) {
                break;
            }
        }
    }
    return 0;
}


#pragma endregion




void WinLongViewer::capStep()
{
    auto dpr = devicePixelRatio();
    auto imgNew = Util::printScreen(areaX, areaY, areaW, areaH);
    imgNew.setDevicePixelRatio(dpr);
    if (firstCheck) {
        topIgnoreSpan = findTopSameHeight(img, imgNew);
        firstCheck = false;
    }
    QImage imgNewSub = imgNew.copy(0, topIgnoreSpan, imgNew.width(), 160);
    imgNewSub.setDevicePixelRatio(dpr);
    auto img3 = img.convertToFormat(QImage::Format_Grayscale8);
    auto img4 = imgNewSub.convertToFormat(QImage::Format_Grayscale8);
    auto y = findMostSimilarRegionParallel(img3, img4);
    qDebug() << "top" << y;
    QImage imgResult(img.width(), y + imgNew.height(), QImage::Format_ARGB32);
    imgResult.setDevicePixelRatio(dpr);
    QPainter p(&imgResult);
    p.drawImage(0, 0, img);
    if (topIgnoreSpan == 0) {
        p.drawImage(0, y / dpr, imgNew);
    }
    else {
        imgNew = imgNew.copy(0, topIgnoreSpan, imgNew.width(), imgNew.height() - topIgnoreSpan);
        p.drawImage(0, (y+ topIgnoreSpan) / dpr, imgNew);
    }
    
    img = imgResult;
    p.end();
    adjustPosSize();    
}

void WinLongViewer::capOneStep()
{
    POINT pt;
    GetCursorPos(&pt);
    auto hwnd = WindowFromPoint(pt);
    if (hwnd != targetHwnd) return;

    INPUT input = { 0 };
    input.type = INPUT_MOUSE;
    input.mi.dwFlags = MOUSEEVENTF_WHEEL;
    input.mi.mouseData = -WHEEL_DELTA;
    SendInput(1, &input, sizeof(INPUT));
    QTimer::singleShot(98, [this]() {
        capStep();
    });
}

void WinLongViewer::paintEvent(QPaintEvent* event)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
	auto r = rect().adjusted(0, 0, 0, -36);
	QImage scaledImg = img.scaled(img.size()/ scaleNum *devicePixelRatio(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    p.drawImage(0,0, scaledImg);
}

void WinLongViewer::closeEvent(QCloseEvent* event)
{
    deleteLater();
    qApp->exit(1);
}

void WinLongViewer::adjustPosSize()
{
    auto dpr = devicePixelRatio();
    setFixedSize(img.width() / scaleNum, img.height() / scaleNum + 32);
    auto x = (areaX + areaW) / dpr;
    auto y = (areaY + areaH) / dpr;
    move(x, y - height());
}
