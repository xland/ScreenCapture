#include "include/core/SkCanvas.h"
#include "include/core/SkShader.h"
#include "include/core/SkPaint.h"
#include "include/core/SkSurface.h"
#include "include/core/SkImageInfo.h"
#include "include/effects/SkShaderMaskFilter.h"

// 使用 SKSL 编写马赛克效果的着色器
const char* mosaicShaderCode = R"(
    uniform shader input;
    uniform float mosaic_size;

    half4 main(vec2 p) {
        // 将坐标映射到马赛克效果的区域
        vec2 mosaicCoord = floor(p / mosaic_size) * mosaic_size;

        // 在马赛克区域内采样原始图像的颜色
        return sample(input, mosaicCoord);
    }
)";

int main() {
    // 创建一个 SkBitmap 作为绘制目标
    SkImageInfo info = SkImageInfo::MakeN32Premul(400, 400);
    SkBitmap bitmap;
    bitmap.allocPixels(info);

    // 创建 SkCanvas，并将其与 SkBitmap 关联
    SkCanvas canvas(bitmap);

    // 设置画笔
    SkPaint paint;
    paint.setStrokeWidth(60);

    // 绘制路径
    SkPath path;
    path.moveTo(50, 50);
    path.lineTo(200, 200);
    path.lineTo(350, 50);

    // 创建马赛克效果的着色器
    SkRuntimeEffect::Options options;
    sk_sp<SkShader> mosaicShader = SkRuntimeEffect::MakeForShader(
        SkString(mosaicShaderCode), nullptr, &options);

    // 设置着色器为画笔的遮罩滤镜
    paint.setMaskFilter(SkShaderMaskFilter::Make(std::move(mosaicShader)));

    // 使用画笔绘制路径
    canvas.drawPath(path, paint);

    // 在实际应用中，你会在每一帧中更新画布并重新绘制场景

    // bitmap 中包含了马赛克效果的图像

    return 0;
}
