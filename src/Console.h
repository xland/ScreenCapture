#pragma once
#include "JS.h"
#include <quickjs.h>

class Console
{
public:
	static void Reg(JSContext* ctx);
	~Console();
	static void Log(const char* str);
	static void Err(const char* str);
	static void Err(const JSValue& val);
private:
	Console();
	static std::string getLogStr(JSContext* ctx, int argc, JSValueConst* argv);
	static JSValue jsLog(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv);
	static JSValue jsErr(JSContext* ctx, JSValueConst thisVal, int argc, JSValueConst* argv);

};