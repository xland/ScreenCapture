#pragma once
#include <thread>
#include <include/Ling.h>

// 录制参数只在 CapVideo.cpp 里构造，头文件用前向声明就够 ——
// VideoMp4.hpp 会拉进整套 MediaFoundation 头，不该让 ToolVideo / WinCap 这些
// 只是引用 CapVideo 的编译单元都跟着吃一遍。
namespace VideoMp4 { struct DESKTOPCAPTUREPARAMS; }
namespace VideoGif { struct GifParam; }

class WinCap;
class ToolVideo;
// 屏幕录制。区域已经由 WinCap 定好了，所以它不是窗口，只管工具条和编码线程。
class CapVideo
{
public:
	CapVideo(WinCap* win);
	~CapVideo();
	// ToolCap 原地换成 ToolVideo
	void makeTool();
	// 宿主窗口要销毁了：停掉录制、收掉工具条
	void dispose();
	bool isRecording() const;
	void startMp4(bool useSpeaker, bool useMic);
	void startGif();
	// 停止录制并返回录好的临时文件路径；没在录制时返回空串
	std::wstring stop();
private:
	WinCap* win;
	std::unique_ptr<ToolVideo> tool;
	std::unique_ptr<VideoMp4::DESKTOPCAPTUREPARAMS> mp4Param;
	std::unique_ptr<VideoGif::GifParam> gifParam;
	std::jthread captureThread;
};
