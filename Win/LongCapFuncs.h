#pragma once
#include <QImage>
#include <QtConcurrent>


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
    if (results.isEmpty()) return 0;
    MatchResult best = *std::min_element(results.begin(), results.end(), [](const MatchResult& a, const MatchResult& b) {
        return a.error < b.error;
        });
    return best.y;
}
