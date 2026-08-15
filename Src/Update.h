#pragma once
#include <include/Ling.h>

// 自动升级。
// 时机：应用回到"只剩托盘图标待命"的状态时（App::trimMemoryLater 里调），才去访问服务端的
// version.json —— 启动时就请求会跟截图抢资源，用户也可能是拿快捷键抓一张就走的。
// 流程：比版本 -> 新版 exe 下到 %appdata%\ScreenCapture -> 再等一个空闲时机弹窗问用户 ->
// 用户点确定就起一段 PowerShell：等本进程退出、把新 exe 覆盖过去、用 --enter=tray 重新拉起来。
// 请求控制得很死：用完即走模式不查、一天最多查一次、一次运行最多查一次。
class Update
{
public:
	// 每次回到空闲状态都会调，真正干活的条件见上。多调无害
	static void checkLater();
};
