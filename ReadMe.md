![](./Doc/banner.png)


# Features

English:

- Multi-screen capture, Cross screen shot.
- Window area highlight.
- Camera aperture,Copy pixel color:RGB(`Ctrl+R`),HEX(`Ctrl+H`).
- Draw circle(`Shift` down),ellipse,square(`Shift` down),rectangle,arrow,ordered number by diffrent colors (fill or not fill).
- Draw curve,straight line(`Shift` down),opacity line,mosaic,eraser,text(`Mouse Wheel` to change font size).
- Modify the drawn elements at any time(`Mouse Over` the element and wait `1.6` second).
- Undo(`Ctrl+Z`) Redo(`Ctrl+Y`) support.
- Pin the cut area on desktop(`Ctrl+P`).
- Save to file(`Ctrl+S`) or clipboard(`Ctrl+C`).
- Very fast,Small memory footprint,Low CPU useage.
- Only one small executable file (with out any dll).
- Can be easily integrated into any program.
- Powered by Skia.

Chinese:

- 多屏幕截图、跨屏幕截图。
- 窗口区域高亮。
- 取景框，支持快捷键复制 RGB 颜色 （Ctrl+R）和 HEX 颜色（Ctrl+H）。
- 画椭圆、正圆（`Shift`按下）、矩形、正方形（`Shift`按下）、箭头、排序标号（填充、非填充、不同颜色、不同线框粗细）。
- 画曲线、直线（`Shift`按下）、半透明直线、马赛克、橡皮擦、文本（`滚轮`调整文字大小）。
- 可以随时修改已绘制的元素（鼠标移到元素上等待 `1.6` 秒）。
- 上一步（`Ctrl+Z`）、下一步（`Ctrl+Y`）。
- 在新窗口中钉住截图区域（`Ctrl+P`）。
- 保存成文件（`Ctrl+S`）、保存到剪切板（`Ctrl+C`）。
- 非常快、低内存消耗、低 CPU 使用率。
- 非常小、仅一个可执行文件（4.9M），无需安装，不依赖任何动态链接库。
- 可以非常容易的集成到其他应用中。
- 基于Skia。


# Download

[Release](https://github.com/xland/ScreenCapture/releases/) (4.9M)

# Integration

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
    /// 4 quit when copy rgb color
    /// 5 quit when copy hex color
    /// 6 quit when save to file
    /// 7 quit when save to clipboard
    console.log("the quit code is:",code)
});
```

# Support OS

- Windows 10 1607 or Later

# HotKey

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
^!A::Run "D:\path\to\ScreenCapture.exe"
```
- Double Click the `.ahk` file,then you can start the `ScreenCapture.exe` by press `Ctrl+Alt+A`.
- The `.ahk` script will be executed when the system startup.

# Licenses

This project is under the [MIT license](./licence).

