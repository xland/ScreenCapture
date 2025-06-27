简体中文 | [English](./ReadMe.md)

![banner](./Doc/banner.png)

## 特性

- 跨屏幕截图、滚动截图（截长图）、高分屏支持、窗口区域高亮。
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

### 参数说明

所有命令行参数都是**可选的**，无参数启动时将显示完整的截图界面。

### 基础参数

**--path（可选）**：设置本次截图的文件保存路径

```powershell
//如果路径是目录，则保存文件时会自动生成文件名
ScreenCapture.exe --path:"D:\\doc"

//如果路径包含文件名，则按此文件名保存文件（文件存在则覆盖）
//目前只支持PNG格式文件
ScreenCapture.exe --path:"D:\\doc\\test.png"
```

**--lang（可选）**：设置应用程序的语言

```powershell
//支持简体中文(zhcn)和英文(en)，默认为简体中文
ScreenCapture.exe --lang:en
ScreenCapture.exe --lang:zhcn
```

**--comp（可选）**：保存图像时进行压缩

```powershell
//格式：--comp:图像质量,缩放百分比
//图像质量取值范围：-1到9的整数
//  0：最小压缩级别，文件最大
//  9：最大压缩，文件最小
// -1：默认压缩级别，中等压缩
//缩放百分比取值范围：1到100的整数
//注意：图像存入剪贴板时，图像质量参数无效
ScreenCapture.exe --comp:6,60
```

### 截图参数

**--cap（可选）**：截图功能，与--pin参数互斥

**区域截图**：

```powershell
//截取指定区域：x,y为左上角坐标，width,height为宽高
//保存到剪贴板（clipboard参数可选，不指定时保存到文件）
ScreenCapture.exe --cap:area,10,10,500,600,clipboard

//保存到指定路径
ScreenCapture.exe --cap:area,10,10,500,600 --path:"D:\\doc"
```

**全屏截图**：

```powershell
//截取整个屏幕并保存到剪贴板（clipboard参数可选，不指定时保存到文件）
ScreenCapture.exe --cap:fullscreen,clipboard

//截取整个屏幕并保存到指定路径
ScreenCapture.exe --cap:fullscreen --path:"D:\\doc"
```

**自定义截图**：

```powershell
//用户拖拽鼠标确定截图区域后保存到剪贴板（clipboard参数可选，不指定时保存到文件）
ScreenCapture.exe --cap:custom,clipboard

//用户拖拽鼠标确定截图区域后保存到指定路径
ScreenCapture.exe --cap:custom --path:"D:\\doc"
```

**长截图**：

```powershell
//截长图功能，窗口只有四个控制按钮：pin,clipboard,save,close
//也支持通过--tool指令控制
ScreenCapture.exe --cap:long
```

### 钉图参数

**--pin（可选）**：钉图功能，与--cap参数互斥

**钉住剪切板图像**：

```powershell
//钉住剪贴板内的图像，x,y为钉图窗口坐标（可选）
//如果省略坐标，默认坐标为100,100
ScreenCapture.exe --pin:clipboard,100,100
ScreenCapture.exe --pin:clipboard
```

**钉住文件图像**：

```powershell
//钉住指定的图像文件，x,y为钉图窗口坐标（可选）
//如果省略坐标，默认坐标为100,100
ScreenCapture.exe --pin:file,"D:\\test.png",100,100
ScreenCapture.exe --pin:file,"D:\\test.png"
```

**钉住区域图像**：

```powershell
//钉住桌面某个区域的图像
//格式：--pin:area,x,y,width,height,windowX,windowY
//前四个参数为截取区域，后两个为钉图窗口坐标（可选）
//如果省略窗口坐标，默认坐标为100,100
ScreenCapture.exe --pin:area,100,100,500,600,800,10
ScreenCapture.exe --pin:area,100,100,500,600
```

### 界面定制参数

**--tool（可选）**：控制主工具栏按钮的顺序、位置及显隐情况

```powershell
//可用的工具名称：
//绘图工具：rect(矩形), ellipse(椭圆), arrow(箭头), number(序号), line(线条), text(文本), mosaic(马赛克), eraser(橡皮擦)
//操作工具：undo(撤销), redo(重做), pin(钉图), clipboard(剪贴板), save(保存), close(关闭)
//分隔符：|
ScreenCapture.exe --tool:"rect,ellipse,arrow,number,line,text,mosaic,eraser,|,undo,redo,|,pin,clipboard,save,close"

//简化工具栏示例
ScreenCapture.exe --tool:"rect,arrow,|,clipboard,save,close"
```

### 使用示例

```powershell
//无参数启动，显示完整截图界面
ScreenCapture.exe

//英文界面 + 自定义保存路径
ScreenCapture.exe --lang:en --path:"D:\\screenshots"

//快速截图到剪贴板
ScreenCapture.exe --cap:custom,clipboard

//高质量截图保存
ScreenCapture.exe --cap:area,0,0,1920,1080 --comp:-1,100 --path:"D:\\screenshot.png"

//简化工具栏的长截图
ScreenCapture.exe --cap:long --tool:"pin,clipboard,save,close"
```

- `--cap`与`--pin`参数互斥，其他参数可组合使用。
- 一个独立的参数以 `--` 开头，参数内不能出现空格，参数和参数之间以空格分割。
- 在参数里使用文件路径则需根据你的终端来决定使用怎样的路径分隔符：`\\`或`/`。

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

示例2：Win+Ctrl+Alt+P 并通过命令行参数设置截长图
#^!P::Run "path/to/ScreenCapture.exe --cap:long"
*/

^!A::Run "path/to/ScreenCapture.exe"
```

- 双击此 `.ahk` 文件,然后你就可以通过快捷键 `Ctrl+Alt+A` 启动 ScreenCapture.exe 了.
- 此 `.ahk` 脚本会开机自启。

### 方案二：SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/)是一个使用Rust编写的ScreenCapture启动器。

- 支持**开机自启**
- **内置截图程序**，无需额外安装
- 自动注册**全局快捷键**
- 支持自定义默认保存路径
- 防误删程序保护
- 托盘左键单击截图，右键退出

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
