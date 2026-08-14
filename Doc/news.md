这段时间更新的内容：

- 基于自研的 GUI 框架重写（感兴趣可以去看：[Ling](https://github.com/xland/Ling)）。
- 增加 **文字识别插件 OCR**（插件地址：[ImageReader(25MB)](https://github.com/xland/ImageReader)）。
- 增加 **二维码识别** 功能。
- 解决：部分用户录屏异常的问题（录制MP4时）。
- 增加框选截图区域后的功能入口，取消托盘图标菜单功能入口，精简快捷键设置（只保留框选截图区域的快捷键）。
- 支持命令行设置“用完即走”（--auto-quit），无论完成什么工作（截图、录屏、文字识别等），进程随即退出。
- 允许用户调整截图区域的位置和大小（该功能回归了）
- 支持绿色便携特性：允许把配置文件config.json放置到ScreenCapture.exe所在目录。
- 内存占用优化（节省了5MB）
- 解决系统缩放比例DPI变化时导致工具窗口定位不准的问题
- 允许 Ctrl+鼠标滚轮 缩放钉图窗口
- 解决：语言选择对话框会穿越到设置窗口的其他Tab页
- 解决：无法把保存在剪切板内的图像黏贴到Illustrator 2020的问题
- 解决：快捷键设置过于严格，允许设置F1-F12等非输入键为快捷键
- 鼠标在线条、橡皮擦线条、马赛克线条上面移动时，只有移动到这些线条边缘时，才会触发编辑\移动功能
- 按住 Ctrl 框选截图区域，直接进入钉图（标记）窗口
- 钉图窗口，鼠标按下之后马上弹起，不应绘制任何元素
- 支持绿色便携特性：允许把配置文件config.json放置到ScreenCapture.exe所在目录。

开源地址：

[https://github.com/xland/ScreenCapture](https://github.com/xland/ScreenCapture)
[https://gitcode.com/liulun1/ScreenCapture](https://gitcode.com/liulun1/ScreenCapture)

项目说明：

**ScreenCapture** 一个小巧但功能强大的Windows截图工具。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/32220caf5d7445b3ba4aec1d490ee3de.png#pic_center)

## 特性

- 截图、绘图标注、滚动截图（截长图）、录屏。
- 取景框，支持快捷键复制 RGB 颜色（`Ctrl+R`）、 HEX 颜色（`Ctrl+H`）与 CMYK 颜色（`Ctrl+K`）。
- 绘制填充、非填充的椭圆、正圆（按住`Shift`）、矩形、正方形（按住`Shift`）、箭头、标号等。
- 绘制曲线、直线（按住`Shift`）、马赛克、橡皮擦、文本。
- 可以随时修改、删除已绘制的元素（鼠标移到元素上）。
- 撤销（`Ctrl+Z`）、重做（`Ctrl+Y`）、保存为文件（`Ctrl+S`）、保存到剪贴板（`Ctrl+C`或双击）。
- 滚动截图（截长图）、屏幕录制（输出Gif\Mp4格式）。
- 基于[Ling](https://github.com/xland/Ling) GUI 框架。
- 支持文字识别插件 OCR [ImageReader(25MB)](https://github.com/xland/ImageReader)。
- 支持二维码识别功能。
- 运行速度快、内存占用低。
- 体积小、仅一个可执行文件，无需安装，不依赖任何动态链接库。
- 支持通过命令行参数直接启动指定的功能。
- 支持用完即走（进程不驻留在系统中）。
- 多语言支持。

## 下载

[Release](https://github.com/xland/ScreenCapture/releases/) （1MB）

## 支持的操作系统

- Windows 10 1607 or Later

## 编译

- main分支依赖 [Ling](https://github.com/xland/Ling) GUI 框架.
- 使用 Visual Studio 2026（With C++ Desktop Dev Kit）即可编译项目。
- [2.4.25（基于D2D）](https://github.com/xland/ScreenCapture/tree/2.4.25)或 [2.3.3（基于Qt）](https://github.com/xland/ScreenCapture/tree/2.3.3_qt)是以前的稳定分支。

## 命令行

```
// 截图完成后即退出进程。
> ScreenCapture.exe --auto-quit=true
```

## 文字识别插件

下载最新版本的文字识别工具 [ImageReader.exe](https://github.com/xland/ImageReader/releases) (约25MB) 并把此文件放置到ScreenCapture.exe同目录下，或者放置到`%appdata%\ScreenCapture\plugin`目录下，然后重启应用即可使用


​