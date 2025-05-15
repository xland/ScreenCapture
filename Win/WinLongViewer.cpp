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
#include "Tool/ToolLong.h"
#include "WinPin.h"

namespace {
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

    int findMostSimilarRegionParallel(const QImage& img1, const QImage& img2) {

        auto gray1 = img1.convertToFormat(QImage::Format_Grayscale8);
        auto gray2 = img2.convertToFormat(QImage::Format_Grayscale8);
        const int searchHeight = gray1.height() - gray2.height() + 1;
        QList<int> yList;
        for (int y = 0; y < searchHeight; ++y) {
            yList.append(y);
        }
        auto results = QtConcurrent::blockingMapped(yList, [&](int y) { return computeSSDAtY(y, gray1, gray2); });
        MatchResult best = *std::min_element(results.begin(), results.end(), [](const MatchResult& a, const MatchResult& b) {
            return a.error < b.error;
            });
        return best.y;
    }

    size_t findTopSameHeight(const QImage& img1, const QImage& img2) {

        size_t y = 0;
        for (; y < img1.height(); y++)
        {
            bool notSameLine{ false };
            for (size_t x = 0; x < img1.width(); x++)
            {
                if (img1.pixel(x, y) != img2.pixel(x, y)) {
                    notSameLine = true;
                    break;
                }
            }
            if (notSameLine) {
                break;
            }
        }
        return y;
    }
}

WinLongViewer::WinLongViewer(const int& areaX, const int& areaY, const int& areaW, const int& areaH, QWidget *parent) : QMainWindow(parent),
areaX{ areaX }, areaY{ areaY }, areaW{ areaW }, areaH{ areaH }
{

    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_QuitOnClose, false);
    tools = new ToolLong(this);

    img1 = Util::printScreen(areaX, areaY, areaW, areaH);
    imgResult = img1;
    adjustPosSize();
    show();

    POINT pt;
    GetCursorPos(&pt);
    targetHwnd = WindowFromPoint(pt);
    stepTimer = new QTimer(this);
    connect(stepTimer, &QTimer::timeout, this, &WinLongViewer::timerFunc);
    stepTimer->start(800);
    SetCursorPos(pt.x + 1, pt.y + 1); //开始滚动
}

WinLongViewer::~WinLongViewer()
{}

void WinLongViewer::capStep()
{
    auto img2 = Util::printScreen(areaX, areaY, areaW, areaH);
    static int startY{ -1 }, startX{ 999999 }, endY{ -1 }, endX{ -1 };
    {
        if (firstCheck) {
            QList<QPoint> poss;
            for (size_t y = 0; y < img1.height(); y++)
            {
                for (size_t x = 0; x < img1.width(); x++)
                {
                    if (img1.pixel(x, y) != img2.pixel(x, y)) {
                        if (startY == -1) {
                            startY = y;
                        }
                        poss.push_back(QPoint(x, y));
                    }
                }
            }
            endY = poss.last().y();
            for (size_t i = 0; i < poss.size(); i++)
            {
                if (poss[i].x() < startX) startX = poss[i].x();
                if (poss[i].x() > endX) endX = poss[i].x();
            }
            firstCheck = false;
        }
    }
    QImage img11 = img1.copy(startX, startY, endX, img1.height() - startY);
    QImage img22 = img2.copy(startX, startY, endX, 180);
    auto y = findMostSimilarRegionParallel(img11, img22);
    if (y == 0) {
        return;
    }
    auto paintStart = imgResult.height() - (img1.height() - y - startY);
    auto imgResultNew = QImage(imgResult.width(), paintStart +(img2.height()-startY), QImage::Format_ARGB32);
    auto img3 = img2.copy(0, startY, img2.width(), img2.height() - startY);
    
    QPainter p(&imgResultNew);
    p.drawImage(0, 0, imgResult);
    p.drawImage(0, paintStart, img3);
    p.end();
    img1 = img2;
    imgResult = imgResultNew;
    adjustPosSize();
}

void WinLongViewer::saveToClipboard()
{
    Util::imgToClipboard(imgResult);
    close();
    qApp->exit(9);
}

void WinLongViewer::saveToFile()
{
    auto flag = Util::saveToFile(imgResult);
    if (flag) {
        close();
        qApp->exit(8);
    }
}

void WinLongViewer::pin()
{
    QScreen* screen = QGuiApplication::primaryScreen();
    imgResult.setDevicePixelRatio(screen->devicePixelRatio());
    QRect screenGeometry = screen->availableGeometry();
    int x = screenGeometry.x() + (screenGeometry.width() - imgResult.width()) / 2;
    int y = screenGeometry.y() + (screenGeometry.height() - imgResult.height()) / 2;
    new WinPin(QPoint(x,y), imgResult);
    auto win = (WinLong*)parent();
    win->close();
    win->deleteLater();
}

void WinLongViewer::timerFunc()
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
    p.drawImage(0,0, imgSmall);
}

void WinLongViewer::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void WinLongViewer::adjustPosSize()
{
    auto dpr = devicePixelRatio();
    auto sn = imgResult.width() / tools->width();
    auto h = imgResult.height() / sn;
    imgSmall = imgResult.scaled(QSize(tools->width() * dpr, h*dpr), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    imgSmall.setDevicePixelRatio(dpr);
    setFixedSize(imgSmall.width()/dpr, imgSmall.height()/dpr + tools->height());
    auto y = (areaY + areaH) / dpr - height();
    auto x = (areaX + areaW) / dpr + 4;
    auto screen = QGuiApplication::screenAt(QPoint(x + width(),y));
    if (screen) {
        move(x, y);
    }
    else {
        auto x = areaX / dpr - width() - 4;
        move(x, y);
    }
    tools->move(0, height()- tools->height());
    if (!tools->isVisible()) {
        tools->setVisible(true);
    }
}
