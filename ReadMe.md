简体中文 | [English](./Doc/ReadMe.en-US.md) | [Русский](./Doc/ReadMe.ru-RU.md) | [Bahasa Indonesia](./Doc/ReadMe.id-ID.md)

![banner](./Doc/banner.png)

**ScreenCapture** 一个小巧但功能强大的Windows截图工具。

## 特性

- 截图、绘图标注、滚动截图（截长图）、录屏（GIF/MP4）、文字识别（OCR）、二维码识别。
- 取景框（拾色器），支持快捷键复制 RGB 颜色（`Ctrl+R`）、 HEX 颜色（`Ctrl+H`）与 CMYK 颜色（`Ctrl+K`）。
- 绘制椭圆、正圆（按住`Shift`）、矩形、正方形（按住`Shift`）、箭头、标号等。
- 绘制曲线、直线（按住`Shift`）、马赛克、橡皮擦、文本。
- 可以随时修改、删除已绘制的元素（鼠标移到元素上）。
- 撤销（`Ctrl+Z`）、重做（`Ctrl+Y`）、保存为文件（`Ctrl+S`）、保存到剪贴板（`Ctrl+C`或双击）。
- 运行速度快、内存占用低。
- 体积小、仅一个可执行文件，无需安装，不依赖任何动态链接库（文字识别除外）。
- 支持多种命令行参数直接启动指定的功能。
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

// 框选完成后不显示工具条，直接进入指定功能：
// pin 钉图/图像标记
> ScreenCapture.exe --enter=pin
// long 长截图
> ScreenCapture.exe --enter=long
// video 屏幕录制
> ScreenCapture.exe --enter=video
// ocr 文字识别
> ScreenCapture.exe --enter=ocr
// qr 二维码识别
> ScreenCapture.exe --enter=qr
// tray 仅注册托盘图标，不执行任何操作
> ScreenCapture.exe --enter=tray

// 两个参数可以联合使用，比如：不注册托盘图标，截完长图后进程直接退出
> ScreenCapture.exe --enter=long --auto-quit=true
```

## 文字识别插件

下载最新版本的文字识别工具 [ImageReader.exe](https://github.com/xland/ImageReader/releases) (约25MB) 并把此文件放置到ScreenCapture.exe同目录下，或者放置到`%appdata%\ScreenCapture\plugin`目录下，然后重启应用即可使用


## 赞助

<table>
  <tr>
    <td align="center">
      <img alt="支付宝赞助" src="./Doc/alipay.jpg" width="160" height="160">
      <p>支付宝赞助</p>
    </td>
    <td align="center">
      <img alt="微信赞助" src="./Doc/wechat.png" width="160" height="160">
      <p>微信赞助</p>
    </td>
    <td align="center">
      <img alt="作者微信" src="./Doc/author.jpg" width="160" height="160">
      <p>作者微信</p>
    </td>
    <td align="center">
      <img alt="公众号二维码" src="./Doc/gongzhonghao.jpg" width="160" height="160">
      <p>公众号：桌面软件</p>
    </td>
  </tr>
</table>