let spawn = require("child_process").spawn;
let child = spawn("../x64/Debug/ScreenCapture.exe",[],{
    cwd:"./"
})
child.on("close", (code) => {
    console.log("the quit code is:",code)
});