#pragma once
#include <thread>
#include <include/Ling.h>

// 录制参数只在 WinVideo.cpp 里构造，头文件用前向声明就够 ——
// WinVideoMp4.hpp 会拉进整套 MediaFoundation 头，不该让 ToolVideo / Tray 这些
// 只是引用 WinVideo 的编译单元都跟着吃一遍。
namespace WinVideoMp4 { struct DESKTOPCAPTUREPARAMS; }
namespace WinVideoGif { struct GifParam; }

class CutMask;
class ToolVideo;
class WinVideo : public Ling::WinBase
{
public:
	~WinVideo();
	static void init();
	// 退出流程里调：正在录制时先把编码线程停掉，否则线程与设备会卡住
	static void stopIfRecording();
	void startMp4(bool useSpeaker, bool useMic);
	void startGif();
	// 停止录制并返回录好的临时文件路径；没在录制时返回空串
	std::wstring stop();
public:
	std::unique_ptr<CutMask> cutMask;
private:
	WinVideo();
	void onCreated() override;
	void layout() override;
	LRESULT onHitTest(const POINT pos) override;
	BOOL setCursor() override;
	void onDown(POINT pos, bool isRight);
	void onMove(POINT pos);
	void onUp(POINT pos, bool isRight);
	void onKey(UINT key);
	void onClosed();
	void makeTool();
	void drawCursor(HDC hMemDC, const int& width, const int& height);
	void setMouseTransparent(bool transparent);
private:
	std::unique_ptr<ToolVideo> tool;
	bool isFinishCutMask{ false }, isMouseTransparent{ false };
	// Ling 没有 onMouseDrag，靠这个标志在 onMove 里区分"拖框"和"纯移动"
	bool isMouseDown{ false }, isClosed{ false };
	std::unique_ptr<WinVideoMp4::DESKTOPCAPTUREPARAMS> mp4Param;
	std::unique_ptr<WinVideoGif::GifParam> gifParam;
	std::jthread captureThread;
	// 遮罩画在这块画布上
	Ling::Canvas* canvas{ nullptr };
};
