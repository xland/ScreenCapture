
English | [简体中文](./ReadMe.zhcn.md)

![banner](./Doc/banner.png)

# ScreenCapture

A powerful and lightweight screenshot tool for Windows, built with Qt/C++. Supports cross-screen capture, scrolling screenshots, drawing annotations, and extensive command-line integration.

## Features

- **Cross-screen capture**, **scrolling screenshots (long capture)**, **high-DPI support**, and **window area highlighting**.
- **Color picker** with hotkeys for copying RGB (`Ctrl+R`), HEX (`Ctrl+H`), and CMYK (`Ctrl+K`) colors.
- **Drawing tools**: Filled/unfilled ellipses, perfect circles (hold `Shift`), rectangles, squares (hold `Shift`), arrows, and numbered annotations.
- **Line tools**: Curves, straight lines (hold `Shift`), semi-transparent lines, mosaic, eraser, and text.
- **Edit annotations** anytime by hovering over them with the mouse.
- **Undo** (`Ctrl+Z`) and **Redo** (`Ctrl+Y`) support.
- **Pin screenshots** in new windows with mouse wheel zoom functionality.
- **Save to file** (`Ctrl+S`) or **copy to clipboard** (`Ctrl+C` or double-click).
- **High performance** with low memory usage and CPU consumption.
- **Portable** - single executable file, no installation required, no external dependencies.
- **Rich command-line parameters** for easy integration into other applications.

## Download

[Releases](https://github.com/xland/ScreenCapture/releases/) (8MB)

## Command Line Usage

### Parameter Overview

All command-line parameters are **optional**. Launching without parameters shows the full screenshot interface.

### Basic Parameters

**--path (optional)**: Set the save path for screenshots

```powershell
# If path is a directory, filename will be auto-generated
ScreenCapture.exe --path:"D:\\screenshots"

# If path includes filename, save with specified name (overwrites if exists)
# Currently supports PNG format only
ScreenCapture.exe --path:"D:\\screenshots\\capture.png"
```

**--lang (optional)**: Set application language

```powershell
# Supports Simplified Chinese (zhcn) and English (en), default is zhcn
ScreenCapture.exe --lang:en
ScreenCapture.exe --lang:zhcn
```

**--comp (optional)**: Compress images when saving

```powershell
# Format: --comp:quality,scale_percentage
# Quality range: -1 to 9 (integer)
#   0: Minimum compression, largest file
#   9: Maximum compression, smallest file
#  -1: Default compression, medium compression
# Scale percentage range: 1 to 100 (integer)
# Note: Quality parameter has no effect when saving to clipboard
ScreenCapture.exe --comp:6,60
```

### Screenshot Parameters

**--cap (optional)**: Screenshot functionality, mutually exclusive with --pin

**Area Screenshot**:

```powershell
# Capture specified area: x,y are top-left coordinates, width,height are dimensions
# Save to clipboard (clipboard parameter is optional, saves to file if not specified)
ScreenCapture.exe --cap:area,10,10,500,600,clipboard

# Save to specified path
ScreenCapture.exe --cap:area,10,10,500,600 --path:"D:\\screenshots"
```

**Fullscreen Screenshot**:

```powershell
# Capture entire screen and save to clipboard (clipboard parameter is optional)
ScreenCapture.exe --cap:fullscreen,clipboard

# Capture entire screen and save to specified path
ScreenCapture.exe --cap:fullscreen --path:"D:\\screenshots"
```

**Custom Screenshot**:

```powershell
# User drags mouse to define capture area, then saves to clipboard
ScreenCapture.exe --cap:custom,clipboard

# User drags mouse to define capture area, then saves to specified path
ScreenCapture.exe --cap:custom --path:"D:\\screenshots"
```

**Long Screenshot**:

```powershell
# Scrolling screenshot with only four control buttons: pin, clipboard, save, close
# Also supports customization via --tool parameter
ScreenCapture.exe --cap:long
```

### Pin Parameters

**--pin (optional)**: Pin functionality, mutually exclusive with --cap

**Pin Clipboard Image**:

```powershell
# Pin image from clipboard, x,y are pin window coordinates (optional)
# Default coordinates are 100,100 if omitted
ScreenCapture.exe --pin:clipboard,100,100
ScreenCapture.exe --pin:clipboard
```

**Pin Image File**:

```powershell
# Pin specified image file, x,y are pin window coordinates (optional)
# Default coordinates are 100,100 if omitted
ScreenCapture.exe --pin:file,"D:\\image.png",100,100
ScreenCapture.exe --pin:file,"D:\\image.png"
```

**Pin Area Image**:

```powershell
# Pin desktop area image
# Format: --pin:area,x,y,width,height,windowX,windowY
# First four parameters define capture area, last two are pin window coordinates (optional)
# Default window coordinates are 100,100 if omitted
ScreenCapture.exe --pin:area,100,100,500,600,800,10
ScreenCapture.exe --pin:area,100,100,500,600
```

### UI Customization Parameters

**--tool (optional)**: Control toolbar button order, position, and visibility

```powershell
# Available tool names:
# Drawing tools: rect(rectangle), ellipse(ellipse), arrow(arrow), number(numbering), line(line), text(text), mosaic(mosaic), eraser(eraser)
# Action tools: undo(undo), redo(redo), pin(pin), clipboard(clipboard), save(save), close(close)
# Separator: |
ScreenCapture.exe --tool:"rect,ellipse,arrow,number,line,text,mosaic,eraser,|,undo,redo,|,pin,clipboard,save,close"

# Simplified toolbar example
ScreenCapture.exe --tool:"rect,arrow,|,clipboard,save,close"
```

### Usage Examples

```powershell
# Launch without parameters - shows full screenshot interface
ScreenCapture.exe

# English interface with custom save path
ScreenCapture.exe --lang:en --path:"D:\\screenshots"

# Quick screenshot to clipboard
ScreenCapture.exe --cap:custom,clipboard

# High-quality screenshot save
ScreenCapture.exe --cap:area,0,0,1920,1080 --comp:-1,100 --path:"D:\\screenshot.png"

# Long screenshot with simplified toolbar
ScreenCapture.exe --cap:long --tool:"pin,clipboard,save,close"
```

- `--cap` and `--pin` parameters are mutually exclusive; other parameters can be combined.
- Parameters start with `--`, cannot contain spaces, and are separated by spaces.
- Use appropriate path separators (`\\` or `/`) based on your terminal when specifying file paths.

## Integration

Example for Node.js or Electron (similar for Python):

```js
let spawn = require("child_process").spawn;
let child = spawn("./path/to/ScreenCapture.exe");
child.on("close", (code) => {
    /// Exit codes:
    /// 0: undefined
    /// 1: quit by close button
    /// 2: quit by right mouse button
    /// 3: quit by ESC key
    /// 4: quit when copying RGB color
    /// 5: quit when copying HEX color
    /// 6: quit when copying CMYK color
    /// 7: quit when copying mouse position
    /// 8: quit when saving to file
    /// 9: quit when saving to clipboard
    console.log("Exit code:", code);
});
```

## Supported Operating Systems

- Windows 10 1607 or Later

## Hotkey Setup

### Option 1: AutoHotKey

- Download and install [AutoHotKey](https://www.autohotkey.com/).
- Create an `.ahk` script file in `%AppData%/Microsoft/Windows/Start Menu/Programs/Startup` directory with the following content:

```ahk
/*
^  :  Ctrl
!  :  Alt
+  :  Shift
#  :  Win

Example 1: Ctrl+Alt+A 
^!A::Run "path/to/ScreenCapture.exe"

Example 2: Win+Ctrl+Alt+P with long screenshot parameter
#^!P::Run "path/to/ScreenCapture.exe --cap:long"
*/

^!A::Run "path/to/ScreenCapture.exe"
```

- Double-click the `.ahk` file, then you can launch ScreenCapture.exe with `Ctrl+Alt+A`.
- This script will auto-start with Windows.

### Option 2: SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/) is a ScreenCapture launcher written in Rust.

- Supports **auto-start with Windows**
- **Built-in screenshot program**, no additional installation required
- Auto-registers **global hotkeys**
- Supports custom default save paths
- Accidental deletion protection
- Left-click system tray to screenshot, right-click to exit

## Support

<table>
  <tr>
    <td align="center">
      <img alt="Alipay Donation" src="./Doc/alipay.jpg" width="160" height="160">
      <p>Alipay Donation</p>
    </td>
    <td align="center">
      <img alt="WeChat Donation" src="./Doc/wechat.png" width="160" height="160">
      <p>WeChat Donation</p>
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

Thanks to [EV Sign](https://evsign.cn/) for providing digital signature services
