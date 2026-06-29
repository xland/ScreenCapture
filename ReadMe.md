![banner](./Doc/banner.png)

# ScreenCapture

一个功能强大且轻量级的Windows截图工具。

## 特性

- [x] 截图、绘图标注、滚动截图（截长图）、录屏。
- [x] 取景框，支持快捷键复制 RGB 颜色（`Ctrl+R`）、 HEX 颜色（`Ctrl+H`）与 CMYK 颜色（`Ctrl+K`）。
- [x] 绘制填充、非填充的椭圆、正圆（按住`Shift`）、矩形、正方形（按住`Shift`）、箭头、标号等。
- [x] 绘制曲线、直线（按住`Shift`）、马赛克、橡皮擦、文本。
- [x] 可以随时修改、删除已绘制的元素（鼠标移到元素上）。
- [x] 撤销（`Ctrl+Z`）、重做（`Ctrl+Y`）、保存为文件（`Ctrl+S`）、保存到剪贴板（`Ctrl+C`或双击）。
- [x] 滚动截图（截长图）、屏幕录制（输出Gif\Mp4格式）。
- [x] 运行速度快、内存占用低。
- [x] 体积小、仅一个可执行文件，无需安装，不依赖任何动态链接库。
- [ ] 命令行集成
- [ ] 多语言支持
- [ ] 鼠标滚轮放大、缩小截图窗口

## 下载

[Release](https://github.com/xland/ScreenCapture/releases/) （731KB）

## 支持的操作系统

- Windows 10 1607 or Later

## 编译

- 除操作系统自带的库外，本项目不依赖任何第三方库，
- 下载源码，使用 Visual Studio 2026（With C++ Desktop Dev Kit）即可编译项目。

## 命令行

- 命令行中，等号两侧不要有空格
- 以下三种命令可以组合使用，比如：`> enter=long tray=false auto-quit=true`

```
// 启动后立即进入屏幕截图逻辑（默认）
> enter=cap

// 启动后进入长截图（滚动截图）逻辑
> enter=long

// 启动后进入录屏逻辑
> enter=video
```

```
// 显示托盘图标（默认）
> tray=true

// 不显示托盘图标，但截图进程会常驻系统，等待快捷键唤起
> tray=false
```

```
// 截图完成后，进程不退出，随时待命（默认）
> auto-quit=false

// 截图完成后即退出进程。
> auto-quit=true
```

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

感谢 [EV Sign](https://evsign.cn/) 提供数字签名服务
