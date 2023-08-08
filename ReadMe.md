![](./Doc/banner.png)


# Features

- Multi-screen capture, Cross screen shot.
- Draw circle,ellipse,square,rectangle,arrow,number(ordered) by diffrent colors (fill or not fill).
- Draw opacity line,free curve,mosaic,eraser.
- Undo Redo support.
- Save to file,Save to clipboard.
- Camera aperture,Copy pixel color(RGB,HEX).
- Very fast,low memory cust,low cpu useage.
- Only one small executable file (with out any dll).
- Can be easily integrated into any program.

# Download

[Release(only 1.63M)](https://github.com/xland/ScreenCapture/releases/)

# Integration

(With Node.JS Or Electron)

```js
let spawn = require("child_process").spawn;
let child = spawn("../x64/Debug/ScreenCapture.exe")
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

# Licenses

This project is open source under the [MIT license](./licence).

[Blend2D](https://blend2d.com/) drawing engine is used in this project.

Please also comply with Blend2d's [Zlib open source license](https://github.com/blend2d/blend2d/blob/master/LICENSE.md) when using this project.

