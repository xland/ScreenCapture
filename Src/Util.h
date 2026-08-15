#pragma once
#include <include/Ling.h>
#include <array>

// 图像输出相关的工具函数。data 一律要求 BGRA、top-down、行紧凑（步长 = w*4），
// 这也是 WinPin::getImagePixels 交出来的格式。
class Util
{
public:
	// 同时写入 CF_DIBV5（Office / 微信 / WPS 这类原生程序认）和 "PNG" 注册格式
	//（浏览器 / Electron 程序认），两份都带 alpha
	static void saveToClipboard(const int w, const int h, BYTE* data);
	static bool saveToFile(const std::wstring& path, const int w, const int h, BYTE* data);
	// 弹系统另存为对话框，返回空串表示用户取消
	static std::wstring getSaveFilePath(HWND hwnd, const std::wstring& ext = L"png");
	// 以当前时间生成默认文件名，精确到毫秒，避免连续保存时重名
	static std::wstring createFileName(const std::wstring& ext);
	// GDI 抓屏。返回 BGRA、top-down、行紧凑（步长 = w*4），与本类其他函数的入参格式一致
	static std::vector<BYTE> captureScreen(const int x, const int y, const int w, const int h);
	// 把文件路径以 CF_HDROP 写进剪切板，粘贴到资源管理器/聊天窗口就是一个文件
	static void addFileToClipboard(const std::wstring& filePath);
	// cgif 的 config.path 要 char*，这里做 UTF-8 转换（Ling::Util 只有反向的 convertToWStr）
	static std::string convertToStr(const std::wstring& wstr);
	// 把图存成缓存文件，再交给外部插件 ImageReader.exe 做文字识别。插件先在本 exe
	// 同目录找，再找 %appdata%\ScreenCapture\plugin，都找不到就用默认浏览器打开它的
	// release 页面让用户自己下。缓存图由插件读完后自己删。
	static bool openWithImageReader(const int w, const int h, BYTE* data);
	// 用 quirc 识别图里的二维码，返回识别到的内容，没识别到返回空串。
	// 图里有多个码时用换行拼在一起
	static std::wstring decodeQrCode(const int w, const int h, BYTE* data);
	// 读 exe 的版本资源（Resource.rc 里的 FILEVERSION），取前三段。exePath 传空串就是问自己。
	// 取不到（不是 PE 文件、没有版本资源、文件不存在）一律返回 {0,0,0} —— 自动升级里
	// 用它校验下载下来的文件，所以失败不能抛，得是个"明显不对"的值
	static std::array<int, 3> getVerNum(const std::wstring& exePath = L"");
	// 上面那个的 "2.5.9" 形式
	static std::wstring getVer(const std::wstring& exePath = L"");
	// 直接建一个 IDWriteTextLayout。Ling 的 Label/Text 走的是 Composition 节点树，
	// 而 CutMask / WinLong 是在自己的 D2D context 上手绘文字，需要裸的 layout。
	static Microsoft::WRL::ComPtr<IDWriteTextLayout> makeTextLayout(const std::wstring& text, float w, float h, float fontSize);
};
