#include <QFile>
#include <QApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QToolTip>
#include <QSharedMemory>
#include <QMessageBox>
#include "App.h"
#include "NativeRect.h"
#include "Font.h"
#include "../Win/WinFull.h"
#include "../Win/WinPin.h"
#include "../Tool/ToolMain.h"
#include "../Tool/ToolSub.h"
#include "../Tool/ColorCtrl.h"

namespace {
    std::unique_ptr<App> app; 
    QList<QRect> screens;
    QList<QRect> windows;
}
void App::init()
{
    initConfig();
    start();
}
void App::dispose()
{
    app.reset();       
}
void App::start()
{
    NativeRect::init();
    WinFull::init();
}
void App::initConfig()
{
    QString jsonStr = R"888({
  "defaultLang": "en",
  "winPin": [
    {
      "name": "toolBar",
      "zhcn": "工具栏（Ctrl+T）",
      "en": "Toolbar (Ctrl+T)"
    },
    {
      "name": "quit",
      "zhcn": "退出（Esc）",
      "en": "Quit (Esc)"
    }
  ],
  "toolMain": [
    {
      "name": "rect",
      "icon": "e8e8",
      "zhcn": "矩形",
      "en": "Rect"
    },
    {
      "name": "ellipse",
      "icon": "e6bc",
      "zhcn": "圆形",
      "en": "Eellipse"
    },
    {
      "name": "arrow",
      "icon": "e603",
      "zhcn": "箭头",
      "en": "Arrow"
    },
    {
      "name": "number",
      "icon": "e776",
      "zhcn": "标号",
      "en": "Number"
    },
    {
      "name": "line",
      "icon": "e601",
      "zhcn": "线条",
      "en": "Pen"
    },
    {
      "name": "text",
      "icon": "e6ec",
      "zhcn": "文本",
      "en": "Text"
    },
    {
      "name": "mosaic",
      "icon": "e82e",
      "zhcn": "马赛克",
      "en": "Mosaic"
    },
    {
      "name": "eraser",
      "icon": "e6be",
      "zhcn": "橡皮擦",
      "en": "Eraser"
    },
    {
      "name": "splitter"
    },
    {
      "name": "undo",
      "icon": "ed85",
      "zhcn": "撤销",
      "en": "Undo"
    },
    {
      "name": "redo",
      "icon": "ed8a",
      "zhcn": "重做",
      "en": "Redo"
    },
    {
      "name": "splitter"
    },
    {
      "name": "pin",
      "icon": "e6a3",
      "zhcn": "钉住",
      "en": "Pin"
    },
    {
      "name": "clipboard",
      "icon": "e87f",
      "zhcn": "保存到剪切板",
      "en": "Save to Clipboard"
    },
    {
      "name": "save",
      "icon": "e6c0",
      "zhcn": "保存",
      "en": "Save to Disk"
    },
    {
      "name": "close",
      "icon": "e6e7",
      "zhcn": "关闭",
      "en": "Close"
    }
  ],
  "toolSub": {
    "rect": [
      {
        "name": "rectFill",
        "icon": "e602",
        "selectDefault": false,
        "zhcn": "填充矩形",
        "en": "Filled Rect"
      },
      {
        "name": "strokeCtrl",
        "zhcn": "线条粗细：",
        "en": "Stroke Width: ",
        "value": 3,
        "min": 1,
        "max": 20
      },
      {
        "name": "colorCtrl",
        "selectedIndex": 0
      }
    ],
    "ellipse": [
      {
        "name": "ellipseFill",
        "icon": "e600",
        "selectDefault": false,
        "zhcn": "填充椭圆",
        "en": "Filled Ellipse"
      },
      {
        "name": "strokeCtrl",
        "zhcn": "线条粗细：",
        "en": "Stroke Width: ",
        "value": 3,
        "min": 1,
        "max": 20
      },
      {
        "name": "colorCtrl",
        "selectedIndex": 1
      }
    ],
    "arrow": [
      {
        "name": "arrowFill",
        "selectDefault": true,
        "icon": "e604",
        "zhcn": "填充箭头",
        "en": "Filled Arrow"
      },
      {
        "name": "strokeCtrl",
        "zhcn": "箭头大小：",
        "en": "Arrow Size: ",
        "value": 18,
        "min": 12,
        "max": 60
      },
      {
        "name": "colorCtrl",
        "selectedIndex": 2
      }
    ],
    "number": [
      {
        "name": "numberFill",
        "selectDefault": true,
        "icon": "e605",
        "zhcn": "填充标号",
        "en": "Filled Number"
      },
      {
        "name": "colorCtrl",
        "selectedIndex": 3
      }
    ],
    "line": [
      {
        "name": "lineTransparent",
        "selectDefault": false,
        "icon": "e607",
        "zhcn": "半透明线条",
        "en": "Translucent Line"
      },
      {
        "name": "strokeCtrl",
        "zhcn": "线条粗细：",
        "en": "Stroke Width: ",
        "value": 12,
        "min": 6,
        "max": 160
      },
      {
        "name": "colorCtrl",
        "selectedIndex": 4
      }
    ],
    "text": [
      {
        "name": "bold",
        "selectDefault": false,
        "icon": "e634",
        "zhcn": "粗体",
        "en": "Bold"
      },
      {
        "name": "italic",
        "selectDefault": false,
        "icon": "e682",
        "zhcn": "斜体",
        "en": "Italics"
      },
      {
        "name": "strokeCtrl",
        "zhcn": "字体大小：",
        "en": "Font Size: ",
        "value": 16,
        "min": 8,
        "max": 60
      },
      {
        "name": "colorCtrl",
        "selectedIndex": 5
      }
    ],
    "mosaic": [
      {
        "name": "mosaicFill",
        "icon": "e602",
        "selectDefault": false,
        "zhcn": "",
        "en": ""
      },
      {
        "name": "strokeCtrl",
        "zhcn": "线条粗细：",
        "en": "Stroke Width: ",
        "value": 26,
        "min": 16,
        "max": 80
      }
    ],
    "eraser": [
      {
        "name": "eraserFill",
        "icon": "e602",
        "selectDefault": false,
        "zhcn": "矩形橡皮擦",
        "en": "Rectangle Eraser"
      },
      {
        "name": "strokeCtrl",
        "value": 16,
        "zhcn": "线条粗细：",
        "en": "Stroke Width: ",
        "min": 12,
        "max": 60
      }
    ]
  },
  "colorCtrl": {
    "icon": "e61d",
    "iconSelected": "e721",
    "items": [
      {
        "en": "red",
        "zhcn": "红",
        "value": "#CF1322"
      },
      {
        "en": "yellow",
        "zhcn": "黄",
        "value": "#D48806"
      },
      {
        "en": "green",
        "zhcn": "绿",
        "value": "#389E0D"
      },
      {
        "en": "cyan",
        "zhcn": "青",
        "value": "#13C2C2"
      },
      {
        "en": "blue",
        "zhcn": "蓝",
        "value": "#0958D9"
      },
      {
        "en": "purple",
        "zhcn": "紫",
        "value": "#722ED1"
      },
      {
        "en": "pink",
        "zhcn": "粉",
        "value": "#EB2F96"
      },
      {
        "en": "black",
        "zhcn": "黑",
        "value": "#000000"
      }
    ]
  }
})888";
    QJsonDocument document = QJsonDocument::fromJson(jsonStr.toUtf8());
    if (!document.isObject()) {
        qFatal() << "配置文件格式错误";
        return;
    }
    QJsonObject jsonObject = document.object();
    auto lang = jsonObject["defaultLang"].toString();
    WinPin::initData(jsonObject["winPin"].toArray(), lang);
    ToolMain::initData(jsonObject["toolMain"].toArray(), lang);
    ToolSub::initData(jsonObject["toolSub"].toObject(), lang);
    ColorCtrl::initData(jsonObject["colorCtrl"].toObject(), lang);
    Font::initData(jsonObject["icon"].toObject());
}
