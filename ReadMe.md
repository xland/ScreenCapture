**English** | [简体中文](./ReadMe.zhcn.md) | [Русский](./ReadMe.ru.md)

![banner](./Doc/banner.png)

# ScreenCapture

A powerful and lightweight Windows screenshot tool.

## Features

- Screenshot, drawing annotations, scrolling screenshot (long screenshot), screen recording.
- Color picker, supports shortcut keys to copy RGB color (`Ctrl+R`), HEX color (`Ctrl+H`) and CMYK color (`Ctrl+K`).
- Draw filled and unfilled ellipses, perfect circles (hold `Shift`), rectangles, squares (hold `Shift`), arrows, numbered labels, etc.
- Draw curves, straight lines (hold `Shift`), mosaic, eraser, text.
- Modify or delete drawn elements at any time (hover the mouse over an element).
- Undo (`Ctrl+Z`), redo (`Ctrl+Y`), save to file (`Ctrl+S`), save to clipboard (`Ctrl+C` or double-click).
- Scrolling screenshot (long screenshot), screen recording (output Gif\Mp4 format).
- Fast performance with low memory usage.
- Small size, a single executable file, no installation required, does not depend on any dynamic link libraries.
- Supports directly launching specified functions via command-line arguments.
- Supports one-time execution mode (the process will not remain resident in the system).

## Download

[Release](https://github.com/xland/ScreenCapture/releases/) (778KB)

## Supported Operating Systems

- Windows 10 1607 or Later

## Compilation

- This project has no third-party library dependencies other than libraries built into the operating system.
- The project can be compiled with Visual Studio 2026 (installed with the C++ Desktop Development Kit).

## Command Line

- Do not add spaces on either side of the equal sign in command-line arguments.
- The three types of arguments below can be used in combination. Example: 
```
> ScreenCapture.exe enter=long tray=false auto-quit=true
```

```
// Launch and directly enter screenshot mode (default).
> ScreenCapture.exe enter=cap

// Launch and enter long scrolling screenshot mode.
> ScreenCapture.exe enter=long

// Launch and enter screen recording mode.
> ScreenCapture.exe enter=video
```

```
// Show system tray icon (default).
> ScreenCapture.exe tray=true

// Hide system tray icon; 
> ScreenCapture.exe tray=false
```

```
// Keep the process running after screenshot capture.
> auto-quit=false

// Terminate the process immediately after screenshot capture.
> auto-quit=true
```

## Sponsor

<table>
  <tr>
    <td align="center">
      <img alt="Alipay Sponsor" src="./Doc/alipay.jpg" width="160" height="160">
      <p>Alipay Sponsor</p>
    </td>
    <td align="center">
      <img alt="WeChat Sponsor" src="./Doc/wechat.png" width="160" height="160">
      <p>WeChat Sponsor</p>
    </td>
    <td align="center">
      <img alt="Author WeChat" src="./Doc/author.jpg" width="160" height="160">
      <p>Author WeChat</p>
    </td>
    <td align="center">
      <img alt="Official Account QR Code" src="./Doc/gongzhonghao.jpg" width="160" height="160">
      <p>Official Account: Desktop Software</p>
    </td>
  </tr>
</table>

Thanks to [EV Sign](https://evsign.cn/) for providing digital signature services
