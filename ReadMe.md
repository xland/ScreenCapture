简体中文 | [English](./ReadMe.en.md)

![banner](./Doc/banner.png)

## 特性

- 跨屏幕截图、高分屏支持、窗口区域高亮。
- 取景框，支持快捷键复制 RGB 颜色（`Ctrl+R`）、 HEX 颜色（`Ctrl+H`）与 CMYK 颜色（`Ctrl+K`）。
- 绘制填充、非填充的椭圆、正圆（按住`Shift`）、矩形、正方形（按住`Shift`）、箭头、排序标号（`拖拽`调整箭头方向和大小，`滚轮`调整大小）。
- 绘制曲线、直线（按住`Shift`）、半透明直线、马赛克、橡皮擦、文本。
- 可以随时修改已绘制的元素（鼠标移到元素上）。
- 撤销（`Ctrl+Z`）、重做（`Ctrl+Y`）。
- 在新窗口中钉住截图区域，使用鼠标`滚轮`改变钉图窗口大小。
- 保存为文件（`Ctrl+S`）、保存到剪贴板（`Ctrl+C`）。
- 运行速度快、内存占用低、CPU 使用率低。
- 体积小、仅一个可执行文件（8M），无需安装，不依赖任何动态链接库。
- 可以非常容易地集成到其他应用中。

## 下载

[Release](https://github.com/xland/ScreenCapture/releases/) （8M）

## 命令行

**警告**：

1. 下文中的参数都不包含空格等间断字符，即，程序仅接受『一个』整体的参数
2. Windows等系统需注意 `\` 符号的转义问题，需手动将所有 `\` 更改为 `\\` 以避免转义导致的命令错误
3. 程序暂不支持使用 `/` 符号作为路径分隔符

```powershell
//钉住剪切板内的图像，(100,100) 是钉住窗口的坐标
ScreenCapture.exe --pin:clipboard,100,100

//钉住图像文件，(100,100) 是钉住窗口的坐标
ScreenCapture.exe --pin:file,"D:\\test.png",100,100

//钉住桌面某个区域的图像，(100,100,500,600)是区域矩形，(800,10)是钉住窗口的坐标
ScreenCapture.exe --pin:area,100,100,500,600,800,10

//设置本次截图的保存路径与文件名，目前只支持png格式文件
ScreenCapture.exe --path:"D:\\doc\\test.png"

//设置本次截图的保存目录，与 --path 命令互斥
ScreenCapture.exe --dir:"D:\\doc"

//设置应用程序的语言，目前只支持简体中文(zhcn)和英文(en)，默认中文
ScreenCapture.exe --lang:en

//第一个参数6为图像质量，第二个参数60为缩放百分比
//图像质量为 -1~9的整数
//0：无压缩（最小压缩级别，文件最大，保存最快）。
//9：最大压缩（文件最小，保存最慢）。
//-1：默认压缩级别（通常选择中等压缩，平衡大小和速度）。
ScreenCapture.exe --comp:6,60

```

## 集成

(With Node.js Or Electron)

```js
let spawn = require("child_process").spawn;
let child = spawn("./path/to/ScreenCapture.exe")
child.on("close", (code) => {
    /// when code is:
    /// 0 undefined
    /// 1 quit by press close btn;
    /// 2 quit by press right mouse btn;
    /// 3 quit by press esc key;
    /// 4 quit when copy rgb color;
    /// 5 quit when copy hex color;
    /// 6 quit when copy cmyk color;
    /// 7 quit when copy mouse position;
    /// 8 quit when save to file;
    /// 9 quit when save to clipboard;
    console.log("the quit code is:",code)
});
```

## 支持的操作系统

- Windows 10 1607 or Later

## 快捷键

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
#^!P::Run "path/to/ScreenCapture.exe" "--dir:`"D:\doc`""
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

## 赞助

<img alt="ali pay" src="./Doc/alipay.jpg" style="width:160px;height:160px;" /><br />
支付宝支付

<img alt="wechat pay" src="./Doc/wechat.png" style="width:160px;height:160px;" /><br />
微信支付

## 作者

<img alt="wechat qrcode" src="./Doc/author.jpg" style="width:160px;height:160px;" />

无论有没有要聊的话题，无论是不是与此产品有关的话题，都很开心能认识你。