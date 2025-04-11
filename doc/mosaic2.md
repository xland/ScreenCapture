@fragment_shader
void main() {
    // 获取当前像素的位置
    float2 coord = sk_FragCoord.xy;

    // 定义马赛克块的大小
    float blockSize = 10.0;

    // 计算当前像素所在的马赛克块的左上角坐标
    float2 blockStart = floor(coord / blockSize) * blockSize;

    // 计算该马赛克块内的像素总数
    int pixelCount = blockSize * blockSize;

    // 初始化用于存储马赛克块颜色的变量
    float4 blockColor = 0.0;

    // 遍历马赛克块中的所有像素
    for (int y = 0; y < blockSize; y++) {
        for (int x = 0; x < blockSize; x++) {
            // 计算当前像素在原始图像中的坐标
            float2 pixelCoord = blockStart + float2(x, y);

            // 从原始图像中采样颜色
            float4 pixelColor = sample(inputImage, pixelCoord);

            // 累加到马赛克块的颜色中
            blockColor += pixelColor;
        }
    }

    // 计算并返回马赛克块的平均颜色
    emit(blockColor / float4(pixelCount));
}