![](./Doc/banner.png)


# Features

- Multi-screen capture, Cross screen shot.
- Window area highlight.
- Draw circle,ellipse,square,rectangle,arrow,number(ordered) by diffrent colors (fill or not fill).
- Draw opacity line,free curve,mosaic,eraser.
- Undo Redo support.
- Save to file or clipboard.
- Camera aperture,Copy pixel color(RGB,HEX).
- Very fast,Low memory cust,Low CPU useage.
- Only one small executable file (with out any dll).
- Can be easily integrated into any program.

# Download

[Release](https://github.com/xland/ScreenCapture/releases/) (1.63M)

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

# Licenses

This project is under the [MIT license](./licence).

[Blend2D](https://blend2d.com/) is used in this project.

Please also comply with Blend2d's [Zlib open source license](https://github.com/blend2d/blend2d/blob/master/LICENSE.md).

