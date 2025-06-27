English | [简体中文](./ReadMe.zhcn.md)

![banner](./Doc/banner.png)

## Features

- Cross multi HiDPI displays capture.
- Scrolling(long) screenshots.
- Highlights window regions for easy selection.
- Camera aperture, Copy pixel co[SC_Starter](https://github.com/Mikachu2333/sc_starter/) is a process written with Rust and it could assist you for create/change HotKey easily.

- Support **startup on boot**
- **Built-in screenshot program**, no additional installation required
- Automatically register **global hotkeys**
- Support custom default save path
- Program protection against accidental deletion
- Left-click tray to screenshot, right-click to exitGB(`Ctrl+R`), HEX(`Ctrl+H`), CMYK(`Ctrl+K`).
- Draw filled or unfilled shapes: ellipses, circles (hold Shift), rectangles, squares (hold Shift), arrows, and ordered labels.
- Draw curves, straight lines (hold Shift), translucent lines, mosaic, eraser strokes, and text.
- Edit or delete any drawn element at any time (hover to select).
- Undo (Ctrl+Z) and redo (Ctrl+Y) actions.
- Pin screenshots in a new window and resize it with the mouse wheel.
- Save to file (Ctrl+S) or clipboard (Ctrl+C or double-click).
- Very fast, low memory and CPU usage.
- Lightweight: single executable, no installation, no dependencies.
- Rich command-line options for easy integration into other applications.

## Download

[Release](https://github.com/xland/ScreenCapture/releases/) （8M）

## Command line

### Parameter Description

All command-line parameters are **optional**. When launched without parameters, the full screenshot interface will be displayed.

### Basic Parameters

**--path (optional)**: Set the save path for the captured image

```powershell
// If the path is a directory, the filename will be auto-generated.
ScreenCapture.exe --path:"D:\\doc"

// If the path includes a filename, the image will be saved with that name (overwrites if exists).
// Currently, only PNG format is supported.
ScreenCapture.exe --path:"D:\\doc\\test.png"
```

**--lang (optional)**: Set the application language

```powershell
// Currently supports Simplified Chinese (zhcn) and English (en). Default is Simplified Chinese.
ScreenCapture.exe --lang:en
ScreenCapture.exe --lang:zhcn
```

**--comp (optional)**: Compress the image when saving

```powershell
// Format: --comp:quality,scale
// Image quality range: integer from -1 to 9
//  0: lowest compression, largest file
//  9: highest compression, smallest file
// -1: default compression level (medium)
// Scaling percentage range: 1 to 100
// Note: Image quality parameter has no effect when saving to clipboard
ScreenCapture.exe --comp:6,60
```

### Capture Parameters

**--cap (optional)**: Screenshot functionality, mutually exclusive with --pin parameter

**Area capture**:

```powershell
// Capture a specific screen region: x,y is top-left coordinate, width,height is dimensions
// Save to clipboard (clipboard parameter is optional, saves to file if not specified)
ScreenCapture.exe --cap:area,10,10,500,600,clipboard

// Save to specified path
ScreenCapture.exe --cap:area,10,10,500,600 --path:"D:\\doc"
```

**Fullscreen capture**:

```powershell
// Capture the entire screen and save to clipboard (clipboard parameter is optional, saves to file if not specified)
ScreenCapture.exe --cap:fullscreen,clipboard

// Capture the entire screen and save to specified path
ScreenCapture.exe --cap:fullscreen --path:"D:\\doc"
```

**Custom capture**:

```powershell
// User drags mouse to determine screenshot area then save to clipboard (clipboard parameter is optional, saves to file if not specified)
ScreenCapture.exe --cap:custom,clipboard

// User drags mouse to determine screenshot area then save to specified path
ScreenCapture.exe --cap:custom --path:"D:\\doc"
```

**Long screenshot**:

```powershell
// Long screenshot function, window only has four control buttons: pin, clipboard, save, close
// Also supports control via --tool parameter
ScreenCapture.exe --cap:long
```

### Pin Parameters

**--pin (optional)**: Pin functionality, mutually exclusive with --cap parameter

**Pin clipboard image**:

```powershell
// Pin the image from the clipboard, x,y is the window position (optional)
// If position is omitted, the default window position is 100,100
ScreenCapture.exe --pin:clipboard,100,100
ScreenCapture.exe --pin:clipboard
```

**Pin file image**:

```powershell
// Pin a specified image file, x,y is the window position (optional)
// If position is omitted, the default window position is 100,100
ScreenCapture.exe --pin:file,"D:\\test.png",100,100
ScreenCapture.exe --pin:file,"D:\\test.png"
```

**Pin area image**:

```powershell
// Pin a desktop region image
// Format: --pin:area,x,y,width,height,windowX,windowY
// First four parameters are the capture area, last two are the window position (optional)
// If window position is omitted, the default position is 100,100
ScreenCapture.exe --pin:area,100,100,500,600,800,10
ScreenCapture.exe --pin:area,100,100,500,600
```

### Interface Customization Parameters

**--tool (optional)**: Control the order, position, and visibility of main toolbar buttons

```powershell
// Available tool names:
// Drawing tools: rect(rectangle), ellipse, arrow, number, line, text, mosaic, eraser
// Operation tools: undo, redo, pin, clipboard, save, close
// Separator: |
ScreenCapture.exe --tool:"rect,ellipse,arrow,number,line,text,mosaic,eraser,|,undo,redo,|,pin,clipboard,save,close"

// Simplified toolbar example
ScreenCapture.exe --tool:"rect,arrow,|,clipboard,save,close"
```

### Usage Examples

```powershell
// Launch without parameters, show full screenshot interface
ScreenCapture.exe

// English interface + custom save path
ScreenCapture.exe --lang:en --path:"D:\\screenshots"

// Quick screenshot to clipboard
ScreenCapture.exe --cap:custom,clipboard

// High quality screenshot save
ScreenCapture.exe --cap:area,0,0,1920,1080 --comp:-1,100 --path:"D:\\screenshot.png"

// Long screenshot with simplified toolbar
ScreenCapture.exe --cap:long --tool:"pin,clipboard,save,close"
```

### Parameter Rules

- The `--cap` and `--pin` options are mutually exclusive; other options can be combined freely.
- Each standalone option starts with `--`. Options must not contain spaces, and different options are separated by spaces.
- When specifying file paths, use the appropriate path separator (`\\` or `/`) depending on your terminal or shell environment.

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
^  :  Ctrl
!  :  Alt
+  :  Shift
#  :  Win

example1：Ctrl+Alt+A 
^!A::Run "path/to/ScreenCapture.exe"

example2：Win+Ctrl+Alt+P cap long image
#^!P::Run "path/to/ScreenCapture.exe --cap:long"
*/

^!A::Run "path/to/ScreenCapture.exe"
```

- Double Click the `.ahk` file, then you can start the `ScreenCapture.exe` by press `Ctrl+Alt+A`.
- The `.ahk` script will be executed when the system startup.

### SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/) is a process writen with rust and it could assist you for create/change HotKey easily.

- Start ScreenCapture with Hotkey (Automatically rename pics you shotted with time now).
- Pin Clipplboard pic to Screen with HotKey
- Open Settings with Hotkey. (All hotkey settings, etc. are here.)
- Exit with HotKey.

## Sponsorship

<table>
  <tr>
    <td align="center">
      <img alt="Alipay sponsorship" src="./Doc/alipay.jpg" width="160" height="160">
      <p>Alipay Sponsorship</p>
    </td>
    <td align="center">
      <img alt="WeChat sponsorship" src="./Doc/wechat.png" width="160" height="160">
      <p>WeChat Sponsorship</p>
    </td>
    <td align="center">
      <img alt="Author WeChat" src="./Doc/author.jpg" width="160" height="160">
      <p>Author WeChat</p>
    </td>
    <td align="center">
      <img alt="WeChat Official Account" src="./Doc/gongzhonghao.jpg" width="160" height="160">
      <p>Official Account: Desktop Software</p>
    </td>
  </tr>
</table>

Thanks to [EV Sign](https://evsign.cn/) for providing digital signature service
