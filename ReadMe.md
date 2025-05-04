简体中文 | [English](./ReadMe.en.md)

![banner](./Doc/banner.png)

## 特性

- 跨屏幕截图、高分屏支持、窗口区域高亮。
- 取景框，支持快捷键复制 RGB 颜色（`Ctrl+R`）、 HEX 颜色（`Ctrl+H`）与 CMYK 颜色（`Ctrl+K`）。
- 绘制填充、非填充的椭圆、正圆（按住`Shift`）、矩形、正方形（按住`Shift`）、箭头、排序标号。
- 绘制曲线、直线（按住`Shift`）、半透明直线、马赛克、橡皮擦、文本。
- 可以随时修改、删除已绘制的元素（鼠标移到元素上）。
- 撤销（`Ctrl+Z`）、重做（`Ctrl+Y`）。
- 在新窗口中钉住截图区域，使用鼠标滚轮放大、缩小钉图窗口大小。
- 保存为文件（`Ctrl+S`）、保存到剪贴板（`Ctrl+C`或双击鼠标左键）。
- 运行速度快、内存占用低、CPU 使用率低。
- 体积小、仅一个可执行文件，无需安装，不依赖任何动态链接库。
- 丰富的命令行控制参数。可以非常容易地集成到其他应用中。

## 下载

[Release](https://github.com/xland/ScreenCapture/releases/) （8M）

## 命令控制

```powershell
//设置本次截图的文件保存路径。
//如果路径是目录，则保存文件时会自动生成文件名。
//如果路径包含文件名，则按此文件名保存文件（文件存在则覆盖）。
//目前只支持png格式文件
ScreenCapture.exe --path:"D:\\doc\\test.png"
ScreenCapture.exe --path:"D:\\doc"

//截取屏幕某个区域的图像
//100,100是坐标，500,600是宽高
//截取完成后图像写入剪切板
ScreenCapture.exe --cap:area,10,10,500,600,clipboard

//截取屏幕某个区域的图像
//100,100是坐标，500,600是宽高
//截取完成后图像写入指定路径
ScreenCapture.exe --cap:area,10,10,500,600 --path:"D:/doc"

//截取整个屏幕并写入剪切板
ScreenCapture.exe --cap:fullscreen,clipboard

//截取整个屏幕并把图像写入指定路径
ScreenCapture.exe --cap:fullscreen --path:"D:/doc"

//用户拖拽鼠标确定截图区域后，马上把截图区域内的图像写入剪切板
ScreenCapture.exe --cap:custom,clipboard

//用户拖拽鼠标确定截图区域后，马上把截图区域内的图像写入指定路径
ScreenCapture.exe --cap:custom --path:"D:/doc"

//钉住剪切板内的图像，100,100是钉住窗口的坐标
//如忽略窗口坐标，则窗口默认坐标为100,100
ScreenCapture.exe --pin:clipboard,100,100

//钉住图像文件，100,100是钉住窗口的坐标
//如忽略窗口坐标，则窗口默认坐标为100,100
ScreenCapture.exe --pin:file,"D:\\test.png",100,100

//钉住桌面某个区域的图像，(100,100,500,600)是区域矩形，(800,10)是钉住窗口的坐标
//如忽略窗口坐标，则窗口默认坐标为100,100
ScreenCapture.exe --pin:area,100,100,500,600,800,10

//设置应用程序的语言，目前只支持简体中文(zhcn)和英文(en)，默认中文
ScreenCapture.exe --lang:en

//设置保存图像时，对图像进行压缩。
//第一个参数6为图像质量，第二个参数60为缩放百分比
//图像质量取值范围为 -1到9的整数
//0：最小压缩级别，文件最大
//9：最大压缩，文件最小
//-1：默认压缩级别，中等压缩
//缩放百分比取值范围为 1到100的整数
//图像存入剪切板时，图像质量参数无效
ScreenCapture.exe --comp:6,60

```

- 未注明互斥的参数则可以同时使用
- 一个独立的参数以 `--` 开头，参数内不能出现空格，参数和参数之间以空格分割。
- 需根据你的终端来决定使用怎样的路径分隔符：`\\`或`/`。

## 集成

示例为 Node.js 或 Electron，Python类似。

```js
let spawn = require("child_process").spawn;
let child = spawn("./path/to/ScreenCapture.exe");
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

### 方案一：AutoHotKey

- 下载并安装 [AutoHotKey](https://www.autohotkey.com/) 。
- 在`%AppData%/Microsoft/Windows/Start Menu/Programs/Startup`目录下创建一个 `.ahk` 脚本文件，文件内容如下:

```ahk
/*
^  :  Ctrl
!  :  Alt
+  :  Shift
#  :  Win

示例1：Ctrl+Alt+A 
^!A::Run "path/to/ScreenCapture.exe"

示例2：Win+Ctrl+Alt+P 并通过命令行参数设置截图保存路径
#^!P::Run "path/to/ScreenCapture.exe" "--dir:`"D:\doc`""
*/

^!A::Run "path/to/ScreenCapture.exe"
```

- 双击此 `.ahk` 文件,然后你就可以通过快捷键 `Ctrl+Alt+A` 启动 ScreenCapture.exe 了.
- 此 `.ahk` 脚本会随机启动.

### 方案二：SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/)是一个使用Rust编写ScreenCapture启动器。

- 内置截图程序，无需额外安装
- 自动注册全局快捷键
- 支持自定义保存路径
- 支持以时间戳命名
- 文件防删除保护
- 托盘左键单击截图，右键退出
- 自启动设置

## 赞助

<img alt="ali pay" src="./Doc/alipay.jpg" style="width:160px;height:160px;" /><br />
支付宝支付

<img alt="wechat pay" src="./Doc/wechat.png" style="width:160px;height:160px;" /><br />
微信支付

## 作者

<img alt="wechat qrcode" src="./Doc/author.jpg" style="width:160px;height:160px;" />

无论有没有要聊的话题，无论是不是与此产品有关的话题，都很开心能认识你。