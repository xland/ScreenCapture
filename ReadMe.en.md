English | [简体中文](./)

![banner](./Doc/banner.png)

## Features

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

## Download

[Release](https://github.com/xland/ScreenCapture/releases/) （7M）

## Command line

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

## Integration

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

## Support OS

- Windows 10 1607 or Later

## HotKey

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

### SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/) is a process writen with rust and it could assist you for create/change HotKey easily.

It now support

- Start ScreenCapture with Hotkey (Automatically rename pics you shotted with time now).
- Pin Clipplboard pic to Screen with HotKey
- Open Settings with Hotkey. (All hotkey settings, etc. are here.)
- Exit with HotKey.