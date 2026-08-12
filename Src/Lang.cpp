#include "pch.h"
#include <filesystem>
#include "Lang.h"
#include "Setting.h"

std::unique_ptr<Lang> lang;

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

Lang* Lang::get()
{
	return lang.get();
}

std::wstring Lang::get(const std::wstring& keyPath)
{
	auto arr = Ling::Util::splitStr(keyPath, L'.');
	auto obj = lang->langObj.GetNamedObject(arr[0]);
	return std::wstring{ obj.GetNamedString(arr[1]) };
}

void Lang::initLang(const std::wstring& langCode)
{
	if (langCode == L"zh-CN") {
		langObj = JsonObject::Parse(LR"""(
{"cap":{"mark":"图像标记","long":"截长图","video":"录屏","ocr":"文字识别","qrcode":"二维码识别","qrcodeEmpty":"未识别到二维码","qrcodeCopy":"点击确定将内容写入剪切板"},"tray":{"setting":"设置","exit":"退出"},"tool":{"rect":"矩形","ellipse":"圆形","arrow":"箭头","number":"标号","line":"线条","text":"文本","mosaic":"马赛克","eraser":"橡皮擦","undo":"撤销","redo":"重做","clipboard":"剪切板","save":"保存","close":"关闭","pin":"钉住","rectFill":"矩形填充","ellipseFill":"圆形填充","arrowFill":"箭头填充","numberFill":"标号填充","semiTransparent":"半透明","bold":"粗体","italic":"斜体"},"color":{"red":"红","yellow":"黄","green":"绿","cyan":"青","blue":"蓝","purple":"紫","pink":"粉","black":"黑","white":"白"},"video":{"outputMp4":"输出MP4格式文件","outputGif":"输出GIF格式文件","recordSystem":"录制系统声音","recordMic":"录制麦克风声音","startRecord":"开始录制","exit":"退出","stopClipboard":"停止录制，并保存到剪切板","stopFile":"停止录制，并保存到文件","stopExit":"停止录制，并退出"},"setting":{"title":"ScreenCapture设置","common":"通用设置","shortcut":"快捷键","about":"关于","autoStart":"开机自启动：","language":"语言：","getMoreLang":"获取更多语言"},"about":{"version":"版本：","project":"项目：","author":"作者：","wechat":"微信：liulun_007","copySuccess":"复制成功","sysTip":"系统提示"},"shortcut":{"cap":"启动应用：","pressKey":"请按键..."},"long":{"start":"开始","reachedBottom":"已触底，截图停止","tooLong":"图像过长，截图停止"},"util":{"file":"文件"}}
)""");
	}
	else if (langCode == L"en-US") {
		langObj = JsonObject::Parse(LR"""(
{"cap":{"mark":"Annotate Image","long":"Scrolling Screenshot","video":"Record Screen","ocr":"Text Recognition","qrcode":"QR Code Recognition","qrcodeEmpty":"No QR code found","qrcodeCopy":"Click OK to copy the content to the clipboard"},"tray":{"setting":"Settings","exit":"Exit"},"tool":{"rect":"Rectangle","ellipse":"Ellipse","arrow":"Arrow","number":"Number","line":"Line","text":"Text","mosaic":"Mosaic","eraser":"Eraser","undo":"Undo","redo":"Redo","clipboard":"Clipboard","save":"Save","close":"Close","pin":"Pin","rectFill":"Filled Rectangle","ellipseFill":"Filled Ellipse","arrowFill":"Filled Arrow","numberFill":"Filled Number","semiTransparent":"Semi-transparent","bold":"Bold","italic":"Italic"},"color":{"red":"Red","yellow":"Yellow","green":"Green","cyan":"Cyan","blue":"Blue","purple":"Purple","pink":"Pink","black":"Black","white":"White"},"video":{"outputMp4":"Export as MP4 file","outputGif":"Export as GIF file","recordSystem":"Record system audio","recordMic":"Record microphone","startRecord":"Start Recording","exit":"Exit","stopClipboard":"Stop recording and copy to clipboard","stopFile":"Stop recording and save to file","stopExit":"Stop recording and exit"},"setting":{"title":"ScreenCapture Settings","common":"General","shortcut":"Shortcuts","about":"About","autoStart":"Launch at startup:","language":"Language:","getMoreLang":"get more language"},"about":{"version":"Version:","project":"Project:","author":"Author:","wechat":"WeChat: liulun_007","copySuccess":"Copied successfully","sysTip":"Information"},"shortcut":{"cap":"Launch App:","pressKey":"Press keys..."},"long":{"start":"Start","reachedBottom":"Reached the bottom, capture stopped","tooLong":"Image too long, capture stopped"},"util":{"file":"File"}}
)""");
	}
	else
	{
		auto dataPath = Setting::get()->getDataPath();
		dataPath.append(L"Lang");
		for (const auto& entry : std::filesystem::directory_iterator(dataPath)) {
			std::wstring filename = entry.path().filename().wstring();
			if (filename.find(langCode) != std::wstring::npos) {
				auto pathStr = entry.path().wstring();
				std::wstring content = Ling::Util::readFile(pathStr);
				langObj = JsonObject::Parse(content.data());
				break;
			}
		}
	}
}

std::vector<std::pair<std::wstring, std::wstring>> Lang::getSupportedLang()
{
	std::vector<std::pair<std::wstring, std::wstring>> result = { {L"简体中文",L"zh-CN"},{L"English",L"en-US"} };
	auto dataPath = Setting::get()->getDataPath();
	dataPath.append(L"Lang");
	for (const auto& entry : std::filesystem::directory_iterator(dataPath)) {
		std::wstring filename = entry.path().filename().wstring();
		auto arr = Ling::Util::splitStr(filename, L'.');
		if (arr.size() == 3 && arr[2] == L"json") {
			result.push_back({ arr[0] ,arr[1] });
		}
	}
	return result;
}
