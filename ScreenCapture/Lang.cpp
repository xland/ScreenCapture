#include "pch.h"
#include "Lang.h"
#include "Setting.h"

std::unique_ptr<Lang> lang;

namespace {
	// 所有支持的语言（code + 自身语言名称）；新增语言只需在此追加并补全对应字典
	const std::unordered_map<std::wstring,std::wstring> supportedLangs = {
		{ L"zh-cn", L"中文" },
		{ L"en", L"English" },
		{ L"id", L"Bahasa Indonesia" },
	};
}

Lang::Lang()
{
}

Lang::~Lang()
{
}

void Lang::init()
{
	if (!lang) {
		lang = std::make_unique<Lang>();
	}
	lang->setLang(Setting::getLanguage());
}

void Lang::dispose()
{
	lang.reset();
}

const std::wstring& Lang::get(const std::wstring& key)
{
	if (!lang) return L"";
	auto it = lang->dic.find(key);
	if (it == lang->dic.end()) return L"";
	return it->second;
}

const std::wstring& Lang::getLang()
{
	return lang->langCode;
}

const std::unordered_map<std::wstring,std::wstring>& Lang::getLangs()
{
	return supportedLangs;
}

void Lang::setLang(const std::wstring& l)
{
	// 验证是否为受支持的语言，否则回退到默认主语言 zh-cn
	bool supported = false;
	for (const auto& info : supportedLangs) {
		if (info.first == l) { supported = true; break; }
	}
	lang->langCode = supported ? l : L"zh-cn";
	lang->load(lang->langCode);
}

void Lang::load(const std::wstring& l)
{
	dic.clear();
	if (l == L"en") {
		dic = {
			{ L"tray.capScreen", L"Capture Screen" },
			{ L"tray.capLong", L"Scrolling Screenshot" },
			{ L"tray.capVideo", L"Record Screen" },
			{ L"tray.setting", L"Settings" },
			{ L"tray.exit", L"Exit" },
			{ L"tool.rect", L"Rectangle" },
			{ L"tool.ellipse", L"Ellipse" },
			{ L"tool.arrow", L"Arrow" },
			{ L"tool.number", L"Number" },
			{ L"tool.line", L"Line" },
			{ L"tool.text", L"Text" },
			{ L"tool.mosaic", L"Mosaic" },
			{ L"tool.eraser", L"Eraser" },
			{ L"tool.undo", L"Undo" },
			{ L"tool.redo", L"Redo" },
			{ L"tool.clipboard", L"Clipboard" },
			{ L"tool.save", L"Save" },
			{ L"tool.close", L"Close" },
			{ L"tool.pin", L"Pin" },
			{ L"tool.rectFill", L"Filled Rectangle" },
			{ L"tool.ellipseFill", L"Filled Ellipse" },
			{ L"tool.arrowFill", L"Filled Arrow" },
			{ L"tool.numberFill", L"Filled Number" },
			{ L"tool.semiTransparent", L"Semi-transparent" },
			{ L"tool.bold", L"Bold" },
			{ L"tool.italic", L"Italic" },
			{ L"color.red", L"Red" },
			{ L"color.yellow", L"Yellow" },
			{ L"color.green", L"Green" },
			{ L"color.cyan", L"Cyan" },
			{ L"color.blue", L"Blue" },
			{ L"color.purple", L"Purple" },
			{ L"color.pink", L"Pink" },
			{ L"color.black", L"Black" },
			{ L"video.stopRecord", L"Stop Recording" },
			{ L"video.outputMp4", L"Export as MP4 file" },
			{ L"video.outputGif", L"Export as GIF file" },
			{ L"video.recordSystem", L"Record system audio" },
			{ L"video.recordMic", L"Record microphone" },
			{ L"video.startRecord", L"Start Recording" },
			{ L"video.exit", L"Exit" },
			{ L"video.stopClipboard", L"Stop recording and copy to clipboard" },
			{ L"video.stopFile", L"Stop recording and save to file" },
			{ L"video.stopExit", L"Stop recording and exit" },
			{ L"setting.title", L"ScreenCapture Settings" },
			{ L"setting.common", L"General" },
			{ L"setting.shortcut", L"Shortcuts" },
			{ L"setting.about", L"About" },
			{ L"setting.autoStart", L"Launch at startup:" },
			{ L"setting.showTray", L"Show tray icon:" },
			{ L"setting.language", L"Language:" },
			{ L"about.version", L"Version:" },
			{ L"about.project", L"Project:" },
			{ L"about.author", L"Author:" },
			{ L"about.wechat", L"WeChat: liulun_007 (click to copy)" },
			{ L"about.copySuccess", L"Copied successfully" },
			{ L"about.sysTip", L"Information" },
			{ L"shortcut.cap", L"Capture Screen:" },
			{ L"shortcut.long", L"Scrolling Screenshot:" },
			{ L"shortcut.video", L"Record Screen:" },
			{ L"shortcut.pressKey", L"Press keys..." },
			{ L"long.start", L"Start" },
			{ L"long.reachedBottom", L"Reached the bottom, capture stopped" },
			{ L"long.tooLong", L"Image too long, capture stopped" },
			{ L"util.file", L"File" },
		};
	}
	else if (l == L"id") {
		dic = {
			// 托盘菜单
			{ L"tray.capScreen", L"Tangkap Layar" },
			{ L"tray.capLong", L"Tangkap Layar Panjang" },
			{ L"tray.capVideo", L"Rekam Layar" },
			{ L"tray.setting", L"Pengaturan" },
			{ L"tray.exit", L"Keluar" },
			// 主工具条 / 长图工具条按钮
			{ L"tool.rect", L"Persegi" },
			{ L"tool.ellipse", L"Elips" },
			{ L"tool.arrow", L"Panah" },
			{ L"tool.number", L"Nomor" },
			{ L"tool.line", L"Garis" },
			{ L"tool.text", L"Teks" },
			{ L"tool.mosaic", L"Mosaik" },
			{ L"tool.eraser", L"Penghapus" },
			{ L"tool.undo", L"Urungkan" },
			{ L"tool.redo", L"Ulangi" },
			{ L"tool.clipboard", L"Papan Klip" },
			{ L"tool.save", L"Simpan" },
			{ L"tool.close", L"Tutup" },
			{ L"tool.pin", L"Sematkan" },
			// 子工具条按钮
			{ L"tool.rectFill", L"Persegi Terisi" },
			{ L"tool.ellipseFill", L"Elips Terisi" },
			{ L"tool.arrowFill", L"Panah Terisi" },
			{ L"tool.numberFill", L"Nomor Terisi" },
			{ L"tool.semiTransparent", L"Semi Transparan" },
			{ L"tool.bold", L"Tebal" },
			{ L"tool.italic", L"Miring" },
			// 颜色
			{ L"color.red", L"Merah" },
			{ L"color.yellow", L"Kuning" },
			{ L"color.green", L"Hijau" },
			{ L"color.cyan", L"Cyan" },
			{ L"color.blue", L"Biru" },
			{ L"color.purple", L"Ungu" },
			{ L"color.pink", L"Merah Muda" },
			{ L"color.black", L"Hitam" },
			// 录屏工具条提示
			{ L"video.stopRecord", L"Hentikan Rekam" },
			{ L"video.outputMp4", L"Simpan sebagai file MP4" },
			{ L"video.outputGif", L"Simpan sebagai file GIF" },
			{ L"video.recordSystem", L"Rekam Suara Sistem" },
			{ L"video.recordMic", L"Rekam Suara Mikrofon" },
			{ L"video.startRecord", L"Mulai Rekam" },
			{ L"video.exit", L"Keluar" },
			{ L"video.stopClipboard", L"Hentikan & simpan ke papan klip" },
			{ L"video.stopFile", L"Hentikan & simpan ke file" },
			{ L"video.stopExit", L"Hentikan & keluar" },
			// 设置窗口
			{ L"setting.title", L"Pengaturan ScreenCapture" },
			{ L"setting.common", L"Umum" },
			{ L"setting.shortcut", L"Pintasan" },
			{ L"setting.about", L"Tentang" },
			{ L"setting.autoStart", L"Mulai Otomatis:" },
			{ L"setting.showTray", L"Tampilkan Ikon Baki Sistem:" },
			{ L"setting.language", L"Bahasa:" },
			{ L"about.version", L"Versi:" },
			{ L"about.project", L"Proyek:" },
			{ L"about.author", L"Penulis:" },
			{ L"about.wechat", L"WeChat: liulun_007 (klik untuk salin)" },
			{ L"about.copySuccess", L"Berhasil disalin" },
			{ L"about.sysTip", L"Informasi" },
			{ L"shortcut.cap", L"Tangkap Layar:" },
			{ L"shortcut.long", L"Tangkap Layar Panjang:" },
			{ L"shortcut.video", L"Rekam Layar:" },
			{ L"shortcut.pressKey", L"Tekan tombol..." },
			// 滚动截图
			{ L"long.start", L"Mulai" },
			{ L"long.reachedBottom", L"Sudah mencapai dasar, tangkapan dihentikan" },
			{ L"long.tooLong", L"Gambar terlalu panjang, tangkapan dihentikan" },
			// 保存对话框
			{ L"util.file", L"File" },
		};
	}
	else {
		// 默认/主语言：简体中文
		dic = {
			{ L"tray.capScreen", L"屏幕截图" },
			{ L"tray.capLong", L"滚动截图" },
			{ L"tray.capVideo", L"屏幕录制" },
			{ L"tray.setting", L"设置" },
			{ L"tray.exit", L"退出" },
			{ L"tool.rect", L"矩形" },
			{ L"tool.ellipse", L"圆形" },
			{ L"tool.arrow", L"箭头" },
			{ L"tool.number", L"标号" },
			{ L"tool.line", L"线条" },
			{ L"tool.text", L"文本" },
			{ L"tool.mosaic", L"马赛克" },
			{ L"tool.eraser", L"橡皮擦" },
			{ L"tool.undo", L"撤销" },
			{ L"tool.redo", L"重做" },
			{ L"tool.clipboard", L"剪切板" },
			{ L"tool.save", L"保存" },
			{ L"tool.close", L"关闭" },
			{ L"tool.pin", L"钉住" },
			{ L"tool.rectFill", L"矩形填充" },
			{ L"tool.ellipseFill", L"圆形填充" },
			{ L"tool.arrowFill", L"箭头填充" },
			{ L"tool.numberFill", L"标号填充" },
			{ L"tool.semiTransparent", L"半透明" },
			{ L"tool.bold", L"粗体" },
			{ L"tool.italic", L"斜体" },
			{ L"color.red", L"红" },
			{ L"color.yellow", L"黄" },
			{ L"color.green", L"绿" },
			{ L"color.cyan", L"青" },
			{ L"color.blue", L"蓝" },
			{ L"color.purple", L"紫" },
			{ L"color.pink", L"粉" },
			{ L"color.black", L"黑" },
			{ L"video.stopRecord", L"停止录制" },
			{ L"video.outputMp4", L"输出MP4格式文件" },
			{ L"video.outputGif", L"输出GIF格式文件" },
			{ L"video.recordSystem", L"录制系统声音" },
			{ L"video.recordMic", L"录制麦克风声音" },
			{ L"video.startRecord", L"开始录制" },
			{ L"video.exit", L"退出" },
			{ L"video.stopClipboard", L"停止录制，并保存到剪切板" },
			{ L"video.stopFile", L"停止录制，并保存到文件" },
			{ L"video.stopExit", L"停止录制，并退出" },
			{ L"setting.title", L"ScreenCapture设置" },
			{ L"setting.common", L"通用设置" },
			{ L"setting.shortcut", L"快捷键" },
			{ L"setting.about", L"关于" },
			{ L"setting.autoStart", L"开机自启动：" },
			{ L"setting.showTray", L"显示托盘图标：" },
			{ L"setting.language", L"语言：" },
			{ L"about.version", L"版本：" },
			{ L"about.project", L"项目：" },
			{ L"about.author", L"作者：" },
			{ L"about.wechat", L"微信：liulun_007（点击复制）" },
			{ L"about.copySuccess", L"复制成功" },
			{ L"about.sysTip", L"系统提示" },
			{ L"shortcut.cap", L"屏幕截图：" },
			{ L"shortcut.long", L"滚动截图：" },
			{ L"shortcut.video", L"屏幕录制：" },
			{ L"shortcut.pressKey", L"请按键..." },
			{ L"long.start", L"开始" },
			{ L"long.reachedBottom", L"已触底，截图停止" },
			{ L"long.tooLong", L"图像过长，截图停止" },
			{ L"util.file", L"文件" },
		};
	}
}
