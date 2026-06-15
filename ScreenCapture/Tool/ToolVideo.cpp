#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Win/WinCap.h"
#include "Win/WinVideo.h"
#include "ToolBase.h"
#include "ToolVideo.h"
#include "ToolSub.h"
#include "History.h"


ToolVideo::ToolVideo(const int& x, const int& y, const int& w, const int& h, WinVideo* parent) : ToolBase(x, y, w, h), parent{ parent }
{
    btnId = { "speaker", "mic" , "clapper" , "close" };
    btnName = { L"录制系统声音",L"录制麦克风声音",L"开始录制",L"退出" };
}
ToolVideo::~ToolVideo()
{
}
void ToolVideo::onCreated()
{
    btnSize = 32.f * dpi;
    auto fSize{ 15.f * dpi };
    btnLayout.push_back(makeIconLayout(L"\ue654", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue73b", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue660", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue62d", btnSize, btnSize, fSize));

    borderRadius *= dpi;
    paddingTopBottom *= dpi;
    initBrush();
    fSize = 13 * dpi;
    mp4Start = 6.f * dpi;
    float formatW{ 42.f * dpi };
    formatMp4 = makeTextLayout(L"MP4", formatW, h, fSize);
    mp4End = 6.f*dpi + formatW;
    formatGif = makeTextLayout(L"GIF", formatW, h, fSize);
    gifEnd = mp4End + formatW;

    speakerStart = gifEnd + dpi * 5 + 0.6 * dpi;
    micStart = speakerStart + btnSize;
    clapperStart = micStart + btnSize + dpi * 4 + 0.6 * dpi;
    closeStart = clapperStart + btnSize;
}
bool ToolVideo::isBlueForeColor(const int& index)
{
    if (index == selectIndexFormat || hoverIndex==index) {
        return true;
    }
}
BOOL ToolVideo::onCursor()
{
    setCursor(IDC_HAND);
    return TRUE;
}

void ToolVideo::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF));
    float borderRadius{ 4.f * dpi }, paddingTopBottom{ 4.6f * dpi };
    if (selectIndexFormat == 0) {
        D2D1_ROUNDED_RECT rr = { { mp4Start, paddingTopBottom + marginTop, mp4End, h - paddingTopBottom }, borderRadius, borderRadius };
        render->FillRoundedRectangle(rr, brushSelect.Get());
    }
    else if (selectIndexFormat == 1) {
        D2D1_ROUNDED_RECT rr = { { mp4End, paddingTopBottom + marginTop, gifEnd, h - paddingTopBottom }, borderRadius, borderRadius };
        render->FillRoundedRectangle(rr, brushSelect.Get());
    }
    render->DrawTextLayout({ mp4Start,0 }, formatMp4.Get(), (0 == selectIndexFormat || hoverIndex == 0)? brushBlue.Get(): brushIcon.Get());
    render->DrawTextLayout({ mp4End ,0 }, formatGif.Get(), (1 == selectIndexFormat || hoverIndex == 1) ? brushBlue.Get() : brushIcon.Get());
    render->DrawLine({ gifEnd+dpi*3,paddingTopBottom*2 }, { gifEnd+dpi*3,h - paddingTopBottom*2 }, brushSpliter.Get(), 0.6f*dpi);
    paintIcon(speakerStart, getBtnIconLayout("speaker"), hoverIndex == 2, selectSpeaker);
    paintIcon(micStart, getBtnIconLayout("mic"), hoverIndex == 3, selectMic);
    render->DrawLine({ micStart + btnSize + dpi * 2,paddingTopBottom * 2 }, { micStart + btnSize + dpi * 2,h - paddingTopBottom * 2 }, brushSpliter.Get(), 0.6f * dpi);
    paintIcon(clapperStart, getBtnIconLayout("clapper"), hoverIndex == 4, false);
    paintIcon(closeStart, getBtnIconLayout("close"), hoverIndex == 5, false);

  //  int btnIndex{ 0 };
  //  for (auto& layout : btnLayout)
  //  {
		//paintIcon(btnSize * btnIndex, layout.Get(), btnIndex == hoverIndex, btnIndex == selectIndex);
  //      btnIndex += 1;
  //  }
}

void ToolVideo::onMouseDown(const int& x, const int& y, bool isRight)
{
	if (isRight) {
        state = "";
        selectIndex = -1;
        hide();
		return;
	}
    if (hoverIndex < 0) return;
    if (hoverIndex == selectIndexFormat) return;    
    if(hoverIndex < 2)
    {
        selectIndexFormat = hoverIndex;
        refresh();
        return;
    }
    if (hoverIndex == 2) {
        selectSpeaker = !selectSpeaker;
        refresh();
        return;
    }
    if (hoverIndex == 3) {
        selectMic = !selectMic;
        refresh();
        return;
    }

    auto& state = btnId[hoverIndex];
    if (state == "pin") {
        //parent->pin();
        return;
    }
    else if (state == "clipboard") {
        //parent->copyToClipboard();
        return;
    }
    else if (state == "save") {
        //parent->saveToFile();
        return;
    }
    else if (state == "close") {
        App::exit(2);
        return;
    }
}
void ToolVideo::onMouseMove(const int& x, const int& y)
{
    auto index{ -1 };
    if (x > mp4Start && x < mp4End) {
        index = 0;
    }
    else if (x > mp4End && x < gifEnd) {
        index = 1;
    }
    else if (x > speakerStart && x < micStart) {
        index = 2;
    }
    else if (x > micStart && x < clapperStart) {
        index = 3;
    }
    else if (x > clapperStart && x < closeStart) {
        index = 4;
    }
    else if (x > closeStart) {
        index = 5;
    }
    if (index != hoverIndex) {
        hoverIndex = index;
        refresh();
    }
}
void ToolVideo::onMouseLeave()
{
    hoverIndex = -1;
    refresh();
    hideTip();
}

void ToolVideo::onDpiChanged()
{
    btnSize = 32.f * dpi;
    btnLayout.clear();
    auto fSize{ 14.f * dpi };
    btnLayout.push_back(makeIconLayout(L"\ue72d", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue6b6", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue660", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue62d", btnSize, btnSize, fSize));
}