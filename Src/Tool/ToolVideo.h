#pragma once
#include <include/Ling.h>

class WinCap;
class Tip;
// 录屏工具条。两种形态，切换时把 body 的子节点整批重建（同 ToolSub 的做法）：
//   未录制：MP4 / GIF 二选一 + 系统声 / 麦克风开关 + 开始录制 + 退出
//   录制中：计时文字 + 丢弃 / 存文件 / 存剪切板
class ToolVideo : public Ling::WinBase
{
public:
	ToolVideo(WinCap* win);
	~ToolVideo();
	// Ctrl+S / Ctrl+C 从 WinCap 转进来，等价于录制中那两个按钮。
	// 没在录制（还停在设置形态）时没东西可存，返回 false
	bool onSaveKey(bool toClipboard);
private:
	void onCreated() override;
	void onMinMaxInfo(MINMAXINFO* mmi) override;
	void showSetting();
	void showRecording();
	void onFormatClick(int index);
	void onTimerCB(UINT id);
	void startRecord();
	// 停止录制并另存为文件；计时到上限时也走这里
	void saveFile();
	// 停止录制后收工：toClipboard 为真则把文件放进剪切板，否则直接删掉
	void finishRecord(bool toClipboard);
	void updateTimerText();
	void applyFormatStyle();
	// 选中/未选中两套配色，与 ToolSub、ToolMain 的选中效果保持一致
	void applyToggleStyle(Ling::Button* btn, bool selected);
	Ling::Button* makeIconBtn(const std::wstring& code);
	Ling::Node* makeSpliter();
	float settingWidth() const;
	float recordingWidth() const;
	// 按当前 dpi 与当前形态（未录制 / 录制中）把窗口尺寸算出来并应用
	void refreshSize();
private:
	WinCap* win;
	// onDpiChanged 与 onSizeChanged 之间的接力标记，见构造函数里的注释
	bool dpiChanged{ false };

	std::unique_ptr<Tip> tip;
	Ling::Button* btnMp4{ nullptr };
	Ling::Button* btnGif{ nullptr };
	Ling::Button* btnSpeaker{ nullptr };
	Ling::Button* btnMic{ nullptr };
	Ling::Label* timerLabel{ nullptr };
	// 0 = MP4，1 = GIF
	int selectIndex{ 0 };
	int totalSeconds{ 0 };
	bool selectSpeaker{ true }, selectMic{ false }, isRecording{ false };
	// 以下都是逻辑像素，交给 Ling 的 setter 时由其内部乘 dpi
	static constexpr float btnSize{ 32.f };
	static constexpr float formatW{ 42.f };
	static constexpr float timerW{ 112.f };
	static constexpr float spliterW{ 1.f };
	static constexpr UINT tickTimerId{ 100 };
};
