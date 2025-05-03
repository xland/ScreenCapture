The next major version (2.2.x) is developed in the main branch.

The last stable version is available in the [2.1.x](https://github.com/xland/ScreenCapture/tree/2.1.x) branch.

![banner](./Doc/banner.png)

## Features（特性）

### English

- Multi-screen capture, Cross-screen screenshot.
- Window area highlighting.
- Camera aperture, Copy pixel color: RGB(`Ctrl+R`), HEX(`Ctrl+H`), CMYK(`Ctrl+K`).
- Draw circle(`Shift` down), ellipse, square(`Shift` down), rectangle, arrow, ordered numbers in different colors (filled or unfilled).
- Draw curve, straight line(`Shift` down), opacity line, mosaic, eraser, text.
- Modify the drawn elements at any time (when `Mouse Over` the element).
- Undo(`Ctrl+Z`) and Redo(`Ctrl+Y`) support.
- Pin the cut area on desktop, resize pin window using `Mouse Wheel`.
- Save to file(`Ctrl+S`) or clipboard(`Ctrl+C`).
- Very fast, Small memory footprint, Low CPU usage.
- Only one small executable file (7M, without any DLL).
- Can be easily integrated into any program.

### 中文

- 跨屏幕截图、高分屏支持、窗口区域高亮。
- 取景框，支持快捷键复制 RGB 颜色（`Ctrl+R`）和 HEX 颜色（`Ctrl+H`）。
- 绘制填充、非填充的椭圆、正圆（按住`Shift`）、矩形、正方形（按住`Shift`）、箭头、排序标号（`拖拽`调整箭头方向和大小，`滚轮`调整大小）。
- 绘制曲线、直线（按住`Shift`）、半透明直线、马赛克、橡皮擦、文本。
- 可以随时修改已绘制的元素（鼠标移到元素上）。
- 撤销（`Ctrl+Z`）、重做（`Ctrl+Y`）。
- 在新窗口中钉住截图区域，使用鼠标`滚轮`改变钉图窗口大小。
- 保存为文件（`Ctrl+S`）、保存到剪贴板（`Ctrl+C`）。
- 运行速度快、内存占用低、CPU 使用率低。
- 体积小、仅一个可执行文件（7M），无需安装，不依赖任何动态链接库。
- 可以非常容易地集成到其他应用中。

## Download（下载）

[Release](https://github.com/xland/ScreenCapture/releases/) （7M）

## Command line（命令行）

**警告**：

1. 下文中的参数都不包含空格等间断字符，即，程序仅接受『一个』整体的参数
2. Windows等系统需注意 `\` 符号的转义问题，需手动将所有 `\` 更改为 `\\` 以避免转义导致的命令错误
3. 程序暂不支持使用 `/` 符号作为路径分隔符

```powershell
//钉住剪切板内的图像，(100,100) 是钉住窗口的坐标
ScreenCapture.exe --pin:clipboard,100,100

//钉住图像文件，(100,100) 是钉住窗口的坐标
ScreenCapture.exe --pin:file,"D:\\test.png",100,100

//钉住桌面某个区域的图像，(100,100,500,600)是区域矩形，
//(800,10)是钉住窗口的坐标
ScreenCapture.exe --pin:area,100,100,500,600,800,10

//设置本次截图的保存路径与文件名，目前只支持png格式文件
ScreenCapture.exe --path:"D:\\doc\\测试目录\\test.png"

//设置本次截图的保存目录，与 --path 命令互斥
ScreenCapture.exe --dir:"D:\\doc\\测试目录"

//设置应用程序的语言，目前只支持简体中文(zhcn)和英文(en)，默认中文
ScreenCapture.exe --lang:en
```

## Integration（集成）

(With Node.js Or Electron)

```js
let spawn = require("child_process").spawn;
let child = spawn("./path/to/ScreenCapture.exe")
child.on("close", (code) => {
    /// when code is:
    /// 0 undefined
    /// 1 quit by press close btn;
    /// 2 quit by press right mouse btn;
    /// 3 quit by press esc keyboard
    /// 4 quit when copy rgb color;
    /// 5 quit when copy hex color;
    /// 6 quit when copy cmyk color;
    /// 7 quit when copy mouse position;
    /// 8 quit when save to file;
    /// 9 quit when save to clipboard;
    console.log("the quit code is:",code)
});
```

## Support OS（支持的操作系统）

- Windows 10 1607 or Later

## HotKey（快捷键）

### AutoHotKey

- Download [AutoHotKey](https://www.autohotkey.com/) and install it.
- Create a `.ahk` script file in `shell:Startup` folder and set the file's content:

```ahk
/*
You need to save this ahk script to the dir on the following place.
你需要将此文件保存到以下路径中

%AppData%\\Microsoft\\Windows\\Start Menu\\Programs\\Startup
*/

/*
Simple tutorial for ahk.
以下是ahk的简单教程

^  :  Ctrl
!  :  Alt
+  :  Shift
#  :  Win
*/

;This hotkey is Ctrl+Alt+A 
^!A::Run "path/to/ScreenCapture.exe"

;This hotkey is Win+Ctrl+Alt+P
#^!P::Run "path/to/ScreenCapture.exe" "--dir:`"D:\doc\测试目录`""
```

- Double Click the `.ahk` file,then you can start the `ScreenCapture.exe` by press `Ctrl+Alt+A`.
- The `.ahk` script will be executed when the system startup.

### Third-party project

#### SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/)是一个使用Rust编写的快捷键程序，它可以帮助用户简单快捷地修改

[SC_Starter](https://github.com/Mikachu2333/sc_starter/) is a process writen with rust and it could assist you for create/change HotKey easily.

It now support

- Start ScreenCapture with Hotkey (Automatically rename pics you shotted with time now).
- Pin Clipplboard pic to Screen with HotKey
- Open Settings with Hotkey. (All hotkey settings, etc. are here.)
- Exit with HotKey.

## Sponsorship（赞助）

<img alt="ali pay" src="./Doc/alipay.jpg" style="width:160px;height:160px;" /><br />
alipay(支付宝支付)

<img alt="wechat pay" src="./Doc/wechat.png" style="width:160px;height:160px;" /><br />
wechat(微信支付)

## Author（作者）

<img alt="wechat qrcode" src="./Doc/author.jpg" style="width:160px;height:160px;" />

Author's Wechat QR code（作者微信）

Whether there is any topic to talk about, whether it is related to this product or not, I am very happy to meet you.

无论有没有要聊的话题，无论是不是与此产品有关的话题，都很开心能认识你。

## Stargazers over time

[![Stargazers over time](https://starchart.cc/xland/ScreenCapture.svg?variant=adaptive)](https://starchart.cc/xland/ScreenCapture)
