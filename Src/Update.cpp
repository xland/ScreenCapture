#include "pch.h"
#include <filesystem>
#include <fstream>
#include <chrono>
#include <array>
#include <format>
#include <winrt/Windows.Web.Http.h>
#include <winrt/Windows.Web.Http.Filters.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.System.Threading.h>
#include "Update.h"
#include "Setting.h"
#include "Lang.h"
#include "Util.h"

namespace {
	using namespace winrt::Windows::Foundation;
	using namespace winrt::Windows::Data::Json;
	using namespace winrt::Windows::Web::Http;
	using namespace winrt::Windows::Web::Http::Filters;
	using namespace winrt::Windows::Storage::Streams;
	using winrt::Windows::System::Threading::ThreadPoolTimer;

	constexpr std::wstring_view versionUrl{ L"https://screen-capture-1300696206.cos.ap-chengdu.myqcloud.com/version.json" };
	constexpr std::wstring_view exeUrl{ L"https://screen-capture-1300696206.cos.ap-chengdu.myqcloud.com/ScreenCapture.exe" };
	constexpr std::wstring_view releaseUrl{ L"https://github.com/xland/ScreenCapture/releases" };
	// 下载下来的新版 exe 就叫这个名字。跟正式的 exe 名字区分开 —— 用户可能把 exe 改过名，
	// 覆盖的时候按当前 exe 的实际路径来，不能按这个名字来
	constexpr std::wstring_view newExeName{ L"ScreenCapture.update.exe" };

	bool checked{ false };              //一次运行只查一次服务端，查过就不再动了
	UINT_PTR checkTimer{ 0 };
	UINT_PTR promptTimer{ 0 };
	std::filesystem::path newExePath;   //已经下载好的新版 exe，空的表示还没有
	std::wstring newVer;

	//今天是第几天（1970-01-01 以来的天数）。一天最多查一次，就靠这个数记账
	long long today()
	{
		auto day = std::chrono::floor<std::chrono::days>(std::chrono::system_clock::now());
		return day.time_since_epoch().count();
	}

	BOOL CALLBACK onEnumWin(HWND hwnd, LPARAM param)
	{
		if (!IsWindowVisible(hwnd)) return TRUE;
		*reinterpret_cast<int*>(param) += 1;
		return FALSE; //找到一个就够了
	}
	// 空闲 = 屏幕上一个窗口都没有，只剩托盘图标待命。截图窗口、贴图窗口、设置窗口都在本线程上，
	// 挨个问一遍 IsWindowVisible 就够了，不用逐个类去问自己的状态（Ling::App 那个消息窗口
	// 是 HWND_MESSAGE 的，从来不可见，不会误判）
	bool isIdle()
	{
		int count{ 0 };
		EnumThreadWindows(GetCurrentThreadId(), onEnumWin, reinterpret_cast<LPARAM>(&count));
		return count == 0;
	}

	std::filesystem::path selfPath()
	{
		std::vector<wchar_t> buf(MAX_PATH);
		auto len = GetModuleFileName(nullptr, buf.data(), static_cast<DWORD>(buf.size()));
		if (len == 0) return {};
		return std::filesystem::path{ std::wstring{ buf.data(), len } };
	}

	// exe 所在的目录能不能写。装在 Program Files 下（没提权）、或者放在只读介质上就不能，
	// 这种情况下下载了也覆盖不上去，白费服务端的流量，所以下载之前先探一下
	bool canWrite(const std::filesystem::path& dir)
	{
		auto probe = (dir / L"sc.update.probe").wstring();
		auto file = CreateFile(probe.data(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS,
			FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
		if (file == INVALID_HANDLE_VALUE) return false;
		CloseHandle(file); //DELETE_ON_CLOSE，关掉就没了，不用自己删
		return true;
	}

	void removeNewExe()
	{
		if (newExePath.empty()) return;
		std::error_code ec;
		std::filesystem::remove(newExePath, ec);
		newExePath.clear();
	}

	//路径要放进 PowerShell 的单引号字符串里，里面本来有的单引号得翻倍
	std::wstring quotePath(const std::filesystem::path& path)
	{
		std::wstring str = path.wstring();
		size_t pos{ 0 };
		while ((pos = str.find(L'\'', pos)) != std::wstring::npos) {
			str.insert(pos, 1, L'\'');
			pos += 2;
		}
		return L"'" + str + L"'";
	}

	// 起一段 PowerShell 收尾：等本进程退出（exe 开着的时候覆盖不掉自己）、把新 exe 覆盖过去、
	// 用 --enter=tray 把新版本拉起来（只挂托盘，不弹截图窗口）、最后把中间文件和脚本自己删掉。
	// 返回脚本有没有起成功 —— 没起成功就别退出进程了，不然用户以为在升级，其实什么也没发生
	bool startScript(const std::filesystem::path& exePath)
	{
		auto scriptPath = Setting::get()->getDataPath() / L"update.ps1";
		std::wstring pid = std::to_wstring(GetCurrentProcessId());
		std::wstring script;
		script += L"$ErrorActionPreference='SilentlyContinue'\r\n";
		script += L"Wait-Process -Id " + pid + L" -Timeout 60\r\n";
		script += L"Start-Sleep -Milliseconds 500\r\n";
		//直接覆盖，不先删旧的：万一拷贝失败，用户手上至少还有个能用的旧版本
		script += L"Copy-Item -LiteralPath " + quotePath(newExePath) + L" -Destination " + quotePath(exePath) + L" -Force\r\n";
		script += L"if ($?) { Start-Process -FilePath " + quotePath(exePath) + L" -ArgumentList '--enter=tray' }\r\n";
		script += L"Remove-Item -LiteralPath " + quotePath(newExePath) + L" -Force\r\n";
		//脚本删自己：PowerShell 已经把整个文件读完了，删得掉
		script += L"Remove-Item -LiteralPath " + quotePath(scriptPath) + L" -Force\r\n";
		{
			// Windows PowerShell 读没有 BOM 的 .ps1 会按 ANSI 解码，路径里带中文（中文用户名下的
			// %appdata% 就是）会整段乱掉，所以写 UTF-8 + BOM
			std::ofstream file{ scriptPath, std::ios::binary | std::ios::trunc };
			if (!file) return false;
			file << "\xEF\xBB\xBF" << Ling::Util::convertToStr(script);
			if (!file.good()) return false;
		}
		std::wstring cmd{ L"powershell.exe -NoProfile -NonInteractive -ExecutionPolicy Bypass -WindowStyle Hidden -File \"" };
		cmd += scriptPath.wstring() + L"\"";
		STARTUPINFO si{ .cb{ sizeof(STARTUPINFO) } };
		PROCESS_INFORMATION pi{};
		// CREATE_NO_WINDOW：不让 PowerShell 的黑窗口闪出来，整个升级过程对用户无感
		auto flag = CreateProcess(nullptr, cmd.data(), nullptr, nullptr, FALSE,
			CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi);
		if (!flag) return false;
		CloseHandle(pi.hThread);
		CloseHandle(pi.hProcess);
		return true;
	}

	void promptRestart()
	{
		auto title = Lang::get(L"about.sysTip");
		auto text = std::format(L"{} {}\n\n{}", Lang::get(L"update.found"), newVer, Lang::get(L"update.tip"));
		auto btnId = MessageBox(nullptr, text.data(), title.data(),
			MB_OKCANCEL | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
		//不升就把下载的文件删掉，别占着地方。明天空闲时还会再查一次、再问一次
		if (btnId != IDOK) {
			removeNewExe();
			return;
		}
		auto exePath = selfPath();
		if (exePath.empty() || !startScript(exePath)) {
			removeNewExe();
			return;
		}
		//退出让位，剩下的活交给脚本
		Ling::App::get()->quit(0);
	}

	// exe 所在目录写不进去时的提醒：自动升级这条路走不通，问用户要不要自己去下载页面。
	// 不挂定时器反复等空闲 —— 这只是个提醒，等不到就算了，明天还有机会
	void promptNoPermission(const std::wstring& verStr)
	{
		if (!isIdle()) return;
		auto title = Lang::get(L"about.sysTip");
		auto text = std::format(L"{} {}\n\n{}", Lang::get(L"update.found"), verStr, Lang::get(L"update.noPermission"));
		auto btnId = MessageBox(nullptr, text.data(), title.data(), MB_OKCANCEL | MB_ICONINFORMATION | MB_TOPMOST | MB_SETFOREGROUND);
		if (btnId != IDOK) return;
		ShellExecute(nullptr, L"open", releaseUrl.data(), nullptr, nullptr, SW_SHOWNORMAL);
	}

	void promptLater();

	void CALLBACK onPromptTimer(HWND, UINT, UINT_PTR id, DWORD)
	{
		KillTimer(nullptr, id);
		if (promptTimer == id) promptTimer = 0;
		if (newExePath.empty()) return;
		if (!isIdle()) {
			promptLater(); //用户又忙起来了，过会儿再看
			return;
		}
		promptRestart();
	}

	// 弹窗只在空闲时弹：用户正在框选、正在录屏、正在标注的时候抢焦点，等于把人家的活搅了。
	// 所以下载完不马上弹，隔几秒看一眼空不空
	void promptLater()
	{
		if (newExePath.empty() || promptTimer) return;
		promptTimer = SetTimer(nullptr, 0, 5000, onPromptTimer);
	}

	winrt::fire_and_forget doCheck()
	{
		// 网络请求和文件写入都挪到后台线程：这条路上任何一步都可能卡好几秒，
		// 挂在 UI 线程上就是整个应用不动了（热键、托盘一起没反应）
		co_await winrt::resume_background();
		try {
			auto exePath = selfPath();
			if (exePath.empty()) co_return;
			HttpBaseProtocolFilter filter;
			// MostRecent：每次都去问服务端，没变就用本地缓存（一个 304，几乎不耗流量）。
			// 写行为设成 NoCache 是不让响应体进 WinINet 的缓存 —— 那个 exe 有好几 M，没必要存两份
			filter.CacheControl().ReadBehavior(HttpCacheReadBehavior::MostRecent);
			filter.CacheControl().WriteBehavior(HttpCacheWriteBehavior::NoCache);
			HttpClient client{ filter };
			auto op = client.GetStringAsync(Uri{ versionUrl });
			// 10 秒还没回来就撤：这是后台顺手干的活，不值得一直挂着一个连接。
			// Cancel 之后下面的 co_await 会抛，一路走到 catch 里
			auto guard = ThreadPoolTimer::CreateTimer([op](const ThreadPoolTimer&) { op.Cancel(); },
				std::chrono::seconds(10));
			std::wstring body{ co_await op };
			guard.Cancel();
			JsonObject obj{ nullptr };
			//服务端返回的东西不是合法 JSON（被网关塞了个错误页之类）：当没这回事
			if (!JsonObject::TryParse(body, obj)) co_return;
			auto arr = obj.GetNamedArray(L"version", nullptr);
			if (!arr || arr.Size() < 3) co_return;
			std::array<int, 3> remote{ 0,0,0 };
			for (uint32_t i = 0; i < 3; i++) {
				remote[i] = static_cast<int>(arr.GetNumberAt(i)); //不是数字就抛，外面 catch 收着
			}
			//std::array 的比较就是逐个元素比下去，正好是版本号的比法
			if (remote <= Ling::Util::getVerNum()) co_return;
			auto verStr = std::format(L"{}.{}.{}", remote[0], remote[1], remote[2]);
			if (!canWrite(exePath.parent_path())) {
				Ling::App::get()->dq.TryEnqueue([verStr]() { promptNoPermission(verStr); });
				co_return;
			}
			auto buffer = co_await client.GetBufferAsync(Uri{ exeUrl });
			std::vector<BYTE> bytes(buffer.Length());
			DataReader::FromBuffer(buffer).ReadBytes(bytes);
			//不是 PE 文件：多半下回来的是错误页，不是 exe
			if (bytes.size() < 2 || bytes[0] != 'M' || bytes[1] != 'Z') co_return;
			auto dataPath = Setting::get()->getDataPath();
			std::error_code ec;
			std::filesystem::create_directories(dataPath, ec);
			auto target = dataPath / newExeName;
			{
				std::ofstream file{ target, std::ios::binary | std::ios::trunc };
				if (!file) co_return;
				file.write(reinterpret_cast<const char*>(bytes.data()), bytes.size());
			}
			// 下回来的到底是不是服务端说的那个版本：对不上就不敢拿它替换用户的 exe
			if (Ling::Util::getVerNum(target.wstring()) != remote) {
				std::filesystem::remove(target, ec);
				co_return;
			}
			// 回 UI 线程：下面这几个变量和定时器都只在 UI 线程上碰
			Ling::App::get()->dq.TryEnqueue([target, verStr]() {
				newExePath = target;
				newVer = verStr;
				promptLater();
			});
		}
		catch (...) {
			// 网络不通、超时、被取消、服务端返回了个错误码、磁盘写不进去：一律静默。
			// 自动升级是顺手做的事，不该拿弹窗打扰用户，明天空闲时再来
		}
	}

	void CALLBACK onCheckTimer(HWND, UINT, UINT_PTR id, DWORD)
	{
		KillTimer(nullptr, id);
		if (checkTimer == id) checkTimer = 0;
		//这十几秒里用户又开始截图了：这次不查，下次回到空闲还会再来
		if (!isIdle()) return;
		checked = true;
		//查过就记账，成没成功都算 —— 一天一次，失败了也没必要今天反复试
		Setting::get()->setUpdateCheckDay(today());
		doCheck();
	}
}

void Update::checkLater()
{
	//已经下载好了、只是还没找到弹窗的时机：接着等空闲
	if (!newExePath.empty()) {
		promptLater();
		return;
	}
	if (checked || checkTimer) return;
	auto lingApp = Ling::App::get();
	if (!lingApp) return;
	//用完即走模式：人家就是要截一张图就退，别在这种一次性进程里搞后台请求
	if (lingApp->args[L"--auto-quit"] == L"true") return;
	if (Setting::get()->getUpdateCheckDay() >= today()) return; //今天查过了
	// 空闲不代表用户走开了，很可能马上又要截一张。等 15 秒再动手，
	// 这段时间里又开始干活了就作罢（见 onCheckTimer）
	checkTimer = SetTimer(nullptr, 0, 15000, onCheckTimer);
}
