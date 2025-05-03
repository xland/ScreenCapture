The next major version (2.2.x) is developed in the main branch.

The last stable version is available in the [2.1.x](https://github.com/xland/ScreenCapture/tree/2.1.x) branch.

![](./Doc/banner.png)

## Features（特性）

English:

- Multi-screen capture, Cross screen shot.
- Window area highlight.
- Camera aperture,Copy pixel color:RGB(`Ctrl+R`),HEX(`Ctrl+H`),CMYK(`Ctrl+K`).
- Draw circle(`Shift` down),ellipse,square(`Shift` down),rectangle,arrow,ordered number by diffrent colors (fill or not fill).
- Draw curve,straight line(`Shift` down),opacity line,mosaic,eraser,text.
- Modify the drawn elements at any time(when `Mouse Over` the element).
- Undo(`Ctrl+Z`) Redo(`Ctrl+Y`) support.
- Pin the cut area on desktop, resize pin window by `Mouse Wheel`.
- Save to file(`Ctrl+S`) or clipboard(`Ctrl+C`).
- Very fast,Small memory footprint,Low CPU useage.
- Only one small executable file (7M，with out any dll).
- Can be easily integrated into any program.

Chinese:

- 跨屏幕截图、高分屏支持、窗口区域高亮。
- 取景框，支持快捷键复制 RGB 颜色 （`Ctrl+R`）和 HEX 颜色（`Ctrl+H`）。
- 绘制填充、非填充的椭圆、正圆（`Shift`按下）、矩形、正方形（`Shift`按下）、箭头、排序标号（`拖拽`调整箭头方向和大小，`滚轮`调整大小）。
- 画曲线、直线（`Shift`按下）、半透明直线、马赛克、橡皮擦、文本。
- 可以随时修改已绘制的元素（鼠标移到元素上）。
- 上一步（`Ctrl+Z`）、下一步（`Ctrl+Y`）。
- 在新窗口中钉住截图区域,鼠标`滚轮`改变钉图窗口大小。
- 保存成文件（`Ctrl+S`）、保存到剪切板（`Ctrl+C`）。
- 非常快、低内存消耗、低 CPU 使用率。
- 非常小、仅一个可执行文件（7M），无需安装，不依赖任何动态链接库。
- 可以非常容易的集成到其他应用中。


## Download（下载）

[Release](https://github.com/xland/ScreenCapture/releases/) (8M)

## Command line（命令行）

```
//英文，钉住剪切板图片，100,100是钉住窗口的坐标
--lang:en --pin:clipboard,100,100  

//英文，钉住文件图片，100,100是钉住窗口的坐标
--lang:en --pin:file,"D:\test.png",100,100  

//英文，钉住桌面某个区域，100,100,500,600是区域矩形，
//1000,10是钉住窗口的坐标
--lang:en --pin:area,100,100,500,600,1000,10 

//设置默认保存目录
--dir:"D:\doc\测试目录"
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

- Download [AutoHotKey](https://www.autohotkey.com/) and install it.
- Create a `.ahk` script file in `Startup` folder and set the file's content:
```c
/*
C:\Users\[UserName]\AppData\Roaming\Microsoft\Windows\Start Menu\Programs\Startup\ScreenCapture.ahk
^  :  Ctrl
!  :  Alt
+  :  Shift
#  :  Win
The hotkey is Ctrl+Alt+A 
*/
^!A::Run "path/to/ScreenCapture.exe"
```
- Double Click the `.ahk` file,then you can start the `ScreenCapture.exe` by press `Ctrl+Alt+A`.
- The `.ahk` script will be executed when the system startup.


## Sponsorship（赞助）


<img src="./Doc/alipay.jpg" style="width:160px;height:160px;" /><br />
alipay(支付宝支付)


<img src="./Doc/wechat.png" style="width:160px;height:160px;" /><br />
wechat(微信支付)


## Author（作者）

<img src="./Doc/author.jpg" style="width:160px;height:160px;" />

Author's Wechat QR code（作者微信）

Whether there is any topic to talk about, whether it is related to this product or not, I am very happy to meet you.

无论有没有要聊的话题，无论是不是与此产品有关的话题，都很开心能认识你。

## Stargazers over time
[![Stargazers over time](https://starchart.cc/xland/ScreenCapture.svg?variant=adaptive)](https://starchart.cc/xland/ScreenCapture)
