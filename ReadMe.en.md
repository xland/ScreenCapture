English | [简体中文](./)

![banner](./Doc/banner.png)

## Features

- Cross multi HiDPI displays capture.
- Scrolling(long) screenshots.
- Highlights window regions for easy selection.
- Camera aperture, Copy pixel color: RGB(`Ctrl+R`), HEX(`Ctrl+H`), CMYK(`Ctrl+K`).
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

[Release](https://github.com/xland/ScreenCapture/releases/) （7M）

## Command line

```powershell

// Set the save path for the captured image.
// If the path is a directory, the filename will be auto-generated.
// If the path includes a filename, the image will be saved with that name (overwrites if exists).
// Currently, only PNG format is supported.
ScreenCapture.exe --path:"D:\\doc\\test.png"
ScreenCapture.exe --path:"D:\\doc"

// Capture a specific screen region. 10,10 is the top-left coordinate, 500x600 is width and height.
// The captured image will be copied to the clipboard.
ScreenCapture.exe --cap:area,10,10,500,600,clipboard

// Capture a specific screen region and save the image to the specified path.
ScreenCapture.exe --cap:area,10,10,500,600 --path:"D:\\doc"

// Capture the entire screen and copy the image to the clipboard.
ScreenCapture.exe --cap:fullscreen,clipboard

// Capture the entire screen and save the image to the specified path.
ScreenCapture.exe --cap:fullscreen --path:"D:\\doc"

// Let the user drag to select a region, then copy the image to the clipboard.
ScreenCapture.exe --cap:custom,clipboard

// Let the user drag to select a region, then save the image to the specified path.
ScreenCapture.exe --cap:custom --path:"D:\\doc"

// Pin the image from the clipboard to a window. 100,100 is the window position.
// If position is omitted, the default window position is 100,100.
ScreenCapture.exe --pin:clipboard,100,100

// Pin a specified image file. 100,100 is the window position.
ScreenCapture.exe --pin:file,"D:\\test.png",100,100

// Pin a screen region. (100,100,500,600) is the capture area, (800,10) is the window position.
ScreenCapture.exe --pin:area,100,100,500,600,800,10

// Set the application language. Currently supports Simplified Chinese (zhcn) and English (en). Default is Chinese.
ScreenCapture.exe --lang:en

// Compress the image when saving.
// First parameter (6) is image quality, second (60) is scaling percentage.
// Image quality range: integer from -1 to 9
// 0: lowest compression, largest file
// 9: highest compression, smallest file
// -1: default compression level (medium)
// Scaling percentage range: 1 to 100
// Compression has no effect when copying to clipboard.
ScreenCapture.exe --comp:6,60

// Customize the order, position, and visibility of main toolbar buttons (and separators).
ScreenCapture.exe --tool:"rect,ellipse,arrow,number,line,text,mosaic,eraser,|,undo,redo,|,pin,clipboard,save,close"

// Scrolling (long) screenshot.
// The scrolling capture window has only four buttons: `pin`, `clipboard`, `save`, and `close`.  
// It also supports customization via the `--tool` option.
ScreenCapture.exe --cap:long

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
- The `--cap` and `--pin` options are mutually exclusive; other options can be combined freely.
- Each standalone option starts with `--`. Options must not contain spaces, and diffrent options are separated by spaces.
- When specifying file paths, use the appropriate path separator (`\\` or `/`) depending on your terminal or shell environment.


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

- Double Click the `.ahk` file,then you can start the `ScreenCapture.exe` by press `Ctrl+Alt+A`.
- The `.ahk` script will be executed when the system startup.

### SC_Starter

[SC_Starter](https://github.com/Mikachu2333/sc_starter/) is a process writen with rust and it could assist you for create/change HotKey easily.

- Start ScreenCapture with Hotkey (Automatically rename pics you shotted with time now).
- Pin Clipplboard pic to Screen with HotKey
- Open Settings with Hotkey. (All hotkey settings, etc. are here.)
- Exit with HotKey.