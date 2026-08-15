#include "pch.h"
#include <filesystem>
#include "Lang.h"
#include "Setting.h"

std::unique_ptr<Lang> lang;

namespace {
	// 列出 %appdata%\ScreenCapture\Lang 下的语言文件。这个目录是用户自己往里放语言文件
	// 才会有的，全新安装的机器上并不存在，而 directory_iterator 碰到不存在的目录会抛
	// filesystem_error —— 一路抛出消息循环就是整个进程直接没了（表现出来就是"点设置没反应"）。
	// 所以这里用带 error_code 的重载，目录不在就当没有额外语言
	std::vector<std::filesystem::path> getLangFiles()
	{
		std::vector<std::filesystem::path> result;
		auto langPath = Setting::get()->getDataPath().append(L"Lang");
		std::error_code ec;
		std::filesystem::directory_iterator it{ langPath, ec }, end{};
		for (; !ec && it != end; it.increment(ec)) {
			result.push_back(it->path());
		}
		return result;
	}

	// 内置的两种语言。第三方语言文件里缺键时也拿 en-US 这份兜底（见 Lang::get）
	constexpr std::wstring_view langZhCN{ LR"""({"cap":{"mark":"图像标记","long":"截长图","video":"录屏","ocr":"文字识别","qrcode":"二维码识别","qrcodeEmpty":"未识别到二维码","qrcodeCopy":"点击确定将内容写入剪切板"},"tray":{"setting":"设置","exit":"退出"},"tool":{"rect":"矩形","ellipse":"圆形","arrow":"箭头","number":"标号","line":"线条","text":"文本","mosaic":"马赛克","eraser":"橡皮擦","undo":"撤销","redo":"重做","clipboard":"剪切板","save":"保存","close":"关闭","pin":"钉住","rectFill":"矩形填充","ellipseFill":"圆形填充","arrowFill":"箭头填充","numberFill":"标号填充","semiTransparent":"半透明","bold":"粗体","italic":"斜体"},"color":{"red":"红","yellow":"黄","green":"绿","cyan":"青","blue":"蓝","purple":"紫","pink":"粉","black":"黑","white":"白"},"video":{"outputMp4":"输出MP4格式文件","outputGif":"输出GIF格式文件","recordSystem":"录制系统声音","recordMic":"录制麦克风声音","startRecord":"开始录制","exit":"退出","stopClipboard":"停止录制，并保存到剪切板","stopFile":"停止录制，并保存到文件","stopExit":"停止录制，并退出","recordFailed":"屏幕录制失败：请在系统图形设置中为本程序选择\"节能\"后重试"},"setting":{"title":"ScreenCapture设置","common":"通用设置","shortcut":"快捷键","about":"关于","autoStart":"开机自启动：","language":"语言：","getMoreLang":"获取更多语言"},"about":{"version":"版本：","project":"项目：","author":"作者：","wechat":"微信：liulun_007","copySuccess":"复制成功","sysTip":"系统提示"},"shortcut":{"cap":"启动应用：","pressKey":"请按键..."},"long":{"start":"开始","reachedBottom":"已触底，截图停止","tooLong":"图像过长，截图停止"},"util":{"file":"文件"},"update":{"found":"发现新版本","tip":"新版本已经下载完成，点击\"确定\"立即重启，完成升级","noPermission":"程序所在目录没有写入权限，无法自动升级。点击\"确定\"打开下载页面"}})""" };
	constexpr std::wstring_view langEnUS{ LR"""({"cap":{"mark":"Annotate Image","long":"Scrolling Screenshot","video":"Record Screen","ocr":"Text Recognition","qrcode":"QR Code Recognition","qrcodeEmpty":"No QR code found","qrcodeCopy":"Click OK to copy the content to the clipboard"},"tray":{"setting":"Settings","exit":"Exit"},"tool":{"rect":"Rectangle","ellipse":"Ellipse","arrow":"Arrow","number":"Number","line":"Line","text":"Text","mosaic":"Mosaic","eraser":"Eraser","undo":"Undo","redo":"Redo","clipboard":"Clipboard","save":"Save","close":"Close","pin":"Pin","rectFill":"Filled Rectangle","ellipseFill":"Filled Ellipse","arrowFill":"Filled Arrow","numberFill":"Filled Number","semiTransparent":"Semi-transparent","bold":"Bold","italic":"Italic"},"color":{"red":"Red","yellow":"Yellow","green":"Green","cyan":"Cyan","blue":"Blue","purple":"Purple","pink":"Pink","black":"Black","white":"White"},"video":{"outputMp4":"Export as MP4 file","outputGif":"Export as GIF file","recordSystem":"Record system audio","recordMic":"Record microphone","startRecord":"Start Recording","exit":"Exit","stopClipboard":"Stop recording and copy to clipboard","stopFile":"Stop recording and save to file","stopExit":"Stop recording and exit","recordFailed":"Screen recording failed. Please set this app to \"Power saving\" in the system graphics settings and try again."},"setting":{"title":"ScreenCapture Settings","common":"General","shortcut":"Shortcuts","about":"About","autoStart":"Launch at startup:","language":"Language:","getMoreLang":"get more language"},"about":{"version":"Version:","project":"Project:","author":"Author:","wechat":"WeChat: liulun_007","copySuccess":"Copied successfully","sysTip":"Information"},"shortcut":{"cap":"Launch App:","pressKey":"Press keys..."},"long":{"start":"Start","reachedBottom":"Reached the bottom, capture stopped","tooLong":"Image too long, capture stopped"},"util":{"file":"File"},"update":{"found":"New version available","tip":"The new version has been downloaded. Click OK to restart and finish the upgrade.","noPermission":"The program folder is not writable, so it can not update itself. Click OK to open the download page."}})""" };
}

Lang::Lang()
{
}

Lang::~Lang()
{
}

void Lang::init()
{
	auto ptr = new Lang();
	lang.reset(ptr);
	lang->initLang(Setting::get()->getLang());
}

void Lang::dispose()
{
	lang.reset();
}

Lang* Lang::get()
{
	return lang.get();
}

std::wstring Lang::get(const std::wstring& keyPath)
{
	// 一路用带默认值的重载：第三方语言文件缺键是常态（程序加了新文案，人家的文件还是老的），
	// 而带默认值的 GetNamedObject/GetNamedString 不抛异常。缺了先找内置的 en-US，
	// 连那儿也没有就把键名本身显示出来 —— 界面上难看，但比崩掉好，也一眼能看出缺哪个键
	auto arr = Ling::Util::splitStr(keyPath, L'.');
	if (arr.size() < 2) return keyPath;
	auto& self = *lang;
	auto obj = self.langObj.GetNamedObject(arr[0], nullptr);
	if (obj) {
		auto text = obj.GetNamedString(arr[1], L"");
		if (!text.empty()) return std::wstring{ text };
	}
	if (!self.fallbackObj) return keyPath;
	auto def = self.fallbackObj.GetNamedObject(arr[0], nullptr);
	if (!def) return keyPath;
	auto text = def.GetNamedString(arr[1], L"");
	if (text.empty()) return keyPath;
	return std::wstring{ text };
}

void Lang::initLang(const std::wstring& langCode)
{
	if (langCode == L"zh-CN") {
		langObj = JsonObject::Parse(langZhCN);
	}
	else if (langCode == L"en-US") {
		langObj = JsonObject::Parse(langEnUS);
	}
	else
	{
		// 第三方语言文件：用户自己放进来的，可能是老版本的（缺新加的键）、也可能手工改坏了。
		// 所以一是用 TryParse（Parse 解析失败直接抛，一路抛出去就是进程没了），
		// 二是把内置的 en-US 留在 fallbackObj 里给 Lang::get 兜底
		fallbackObj = JsonObject::Parse(langEnUS);
		bool loaded{ false };
		for (const auto& entry : getLangFiles()) {
			std::wstring filename = entry.filename().wstring();
			if (filename.find(langCode) == std::wstring::npos) continue;
			auto pathStr = entry.wstring();
			std::wstring content = Ling::Util::readFile(pathStr);
			JsonObject obj{ nullptr };
			if (JsonObject::TryParse(content, obj)) {
				langObj = obj;
				loaded = true;
			}
			break;
		}
		//文件没找到、或者内容不是合法 JSON：整份都用内置的 en-US
		if (!loaded) langObj = fallbackObj;
	}
}

std::vector<std::pair<std::wstring, std::wstring>> Lang::getSupportedLang()
{
	std::vector<std::pair<std::wstring, std::wstring>> result = { {L"简体中文",L"zh-CN"},{L"English",L"en-US"} };
	for (const auto& entry : getLangFiles()) {
		std::wstring filename = entry.filename().wstring();
		auto arr = Ling::Util::splitStr(filename, L'.');
		if (arr.size() == 3 && arr[2] == L"json") {
			result.push_back({ arr[0] ,arr[1] });
		}
	}
	return result;
}
