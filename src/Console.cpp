#include "Console.h"
#include "Util.h"
#include <iostream>
#include <fcntl.h>
#include <Windows.h>
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>


Console::Console()
{
}

Console::~Console()
{
}

void Console::Log(const char* str)
{
    std::cout << "[Log]:" << str << std::endl;
}

void Console::Err(const char* str)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_INTENSITY);
    std::wcout << L"[Err]:" << str << std::endl;
    SetConsoleTextAttribute(hConsole, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void Console::Err(const JSValue& val)
{
    auto ctx = JS::GetCtx();
    auto stack = JS_GetPropertyStr(ctx, val, "stack");
    const char* str = JS_ToCString(ctx, stack);
    if (str) {
        Err(str);
        JS_FreeCString(ctx, str);
    }
    else {
        Err("NO ERR INFO");
    }
}


void Console::Reg(JSContext* ctx)
{
    JSValue globalObj = JS_GetGlobalObject(ctx);
    JSValue console = JS_NewObject(ctx);
    JS_SetPropertyStr(ctx, console, "log", JS_NewCFunction(ctx, &Console::jsLog, "log", 1));
    JS_SetPropertyStr(ctx, console, "info", JS_NewCFunction(ctx, &Console::jsLog, "info", 1));
    JS_SetPropertyStr(ctx, console, "warn", JS_NewCFunction(ctx, &Console::jsErr, "warn", 1));
    JS_SetPropertyStr(ctx, console, "error", JS_NewCFunction(ctx, &Console::jsErr, "error", 1));
    JS_SetPropertyStr(ctx, globalObj, "console", console);
    JS_FreeValue(ctx, globalObj);
}

std::string Console::getLogStr(JSContext* ctx, int argc, JSValueConst* argv)
{
    std::stringstream ss;
    for (int i = 0; i < argc; i++) {
        const char* str = JS_ToCString(ctx, argv[i]);
        if (str) {
            ss << " " << str;
            JS_FreeCString(ctx, str);
        }
    }
    return ss.str();
}

JSValue Console::jsLog(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv) {

    auto str = getLogStr(ctx, argc, argv);
    Log(str.data());
    return JS::MakeVal(0, JS_TAG_UNDEFINED);
}

JSValue Console::jsErr(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv)
{
    auto str = getLogStr(ctx, argc, argv);
    Err(str.data());
    return JS::MakeVal(0, JS_TAG_UNDEFINED);
}
