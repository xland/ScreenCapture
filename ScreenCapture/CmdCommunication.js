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