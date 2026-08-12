#pragma once
#include <include/Ling.h>

class CutMask;
class ToolCap;
class CapLong;
class CapVideo;
// 截图主窗口。铺满整个虚拟桌面，拖框结束后并不马上让位，而是留下来当宿主：
// 选区可以继续调整，选区右下方摆一个 ToolCap，长图和录屏都挂在它身上。
class WinCap:public Ling::WinBase
{
public:
	~WinCap();
	static void init();
	static WinCap* get();
	// 退出流程里调：正在录制时先把编码线程停掉，否则线程与设备会卡住
	static void stopIfRecording();
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> getCutImg();
	// 工具条统一定位规则：右边与选区右边对齐，下方空间够就摆在选区右下方，
	// 不够就摆右上方，上下都不够就盖在选区右下角内部（留一点边距）
	void layoutTool(Ling::WinBase* tool);
	// 整窗让出鼠标：录制中用户要能直接操作被录的应用
	void setMouseTransparent(bool transparent);
	// CapLong 开始滚动之前把选区抠成一个洞，滚轮消息才落得到底下的目标窗口上
	void hollowWin();
	void restoreWin();
	// 下面都是给工具条用的门面 ————————————————
	// ToolCap
	void startPin();
	void startLong();
	void startVideo();
	void saveToFile();
	void copyToClipboard();
	// ToolVideo，转给 capVideo
	void startMp4(bool useSpeaker, bool useMic);
	void startGif();
	std::wstring stopRecord();
	// ToolLong，转给 capLong
	void longPin();
	void longSaveToFile();
	void longCopyToClipboard();
public:
	// CapLong / CapVideo 用的就是这一份选区，它们自己不再框选
	std::unique_ptr<CutMask> cutMask;
private:
	WinCap();
	void onCreated() override;
	void layout() override;
	BOOL setCursor() override;
	LRESULT onHitTest(const POINT pos) override;
	void setPixPos(POINT pos);
	void getPixImg(POINT pos);
	void paintPix(ID2D1DeviceContext* ctx);
	void onKey(UINT key);
	void onDown(POINT pos, bool isRight);
	void onMove(POINT pos);
	void onUp(POINT pos, bool isRight);
	void onClosed();
	void makeToolCap();
	// 进长图 / 录屏阶段的公共动作：收掉底图与工具条，并提到最上层
	void enterLiveStage();
	// 选区内的像素。BGRA、top-down、行紧凑，可以直接喂 Util 的存盘与剪切板
	bool getCutPixels(std::vector<BYTE>& pixels, int& cw, int& ch);
	std::tuple<int, int, int, int> getCMYK(const BYTE& r, const BYTE& g, const BYTE& b);
private:
	// Select 拖框取色 -> Adjust 调整选区并显示 ToolCap -> Long / Video 交给对应的对象
	enum class CapStage { Select, Adjust, Long, Video };
	CapStage stage{ CapStage::Select };
	std::unique_ptr<ToolCap> toolCap;
	std::unique_ptr<CapLong> capLong;
	std::unique_ptr<CapVideo> capVideo;
	Microsoft::WRL::ComPtr<ID2D1Bitmap1> screenImg,pixImg;
	D2D1_RECT_F pixSrcRect{};
	// 铺满窗口的画布，走 swap chain 双缓冲：底图、蒙版、放大镜每帧都重画，
	// 单缓冲会让合成器采到"擦干净还没画完"的中间态
	Ling::Canvas* canvas{ nullptr };
	POINT pixPos;
	bool isPress{ false }, isClosed{ false }, isMouseTransparent{ false };
	// 进长图 / 录屏后不再画底图：底图是拖框那一刻的静态截图，
	// 留着的话录屏和滚动截图拿到的都是这张死图
	bool hideScreenImg{ false };
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushBg;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> brushText;
	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> crossBrush;
};

