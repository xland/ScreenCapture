#include "pch.h"
#include "Util.h"
#include "App.h"
#include "Lang.h"
#include "Win/WinCap.h"
#include "Win/WinVideo.h"
#include "ToolBase.h"
#include "ToolVideo.h"
#include "ToolSub.h"
#include "History.h"


ToolVideo::ToolVideo(const int& x, const int& y, const int& w, const int& h, WinVideo* parent) : ToolBase(x, y, w, h), parent{ parent }
{
    btnId = { L"speaker", L"mic" , L"clapper" , L"close",L"clipboard",L"save" };
}
ToolVideo::~ToolVideo()
{
}
void ToolVideo::onCreated()
{
    selectIndex = 0;
    initTip();
    initBrush();
    onDpiChanged();
}
void ToolVideo::onTimer(const UINT& timerId)
{
    // GIF 上限 6 分钟，MP4 上限 60 分钟
    const int maxMinutes = (selectIndex == 1) ? 6 : 60;
    const int maxSeconds = maxMinutes * 60;
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    auto text = std::format(L"{:02d}:{:02d} / {:02d}:00", minutes, seconds, maxMinutes);
    timerLayout.Reset();
    timerLayout = makeTextLayout(text, 160.f, h, 15.f * dpi);
    refresh();
    if (totalSeconds >= maxSeconds) {
		saveFile();
        return;
    }
    totalSeconds += 1;
}
void ToolVideo::saveFile()
{
    hide();
    killTimer(100);
    auto srcPath = parent->stop();
    auto tarPath = Util::getSaveFilePath(nullptr, selectIndex == 1 ? L"gif" : L"mp4");
    if (!tarPath.empty()) {
        CopyFile(srcPath.data(), tarPath.data(), false);
    }
    DeleteFile(srcPath.data());
    close();
    parent->release();
}
BOOL ToolVideo::onCursor()
{
    if (hoverIndex == 9) {
        setCursor(IDC_ARROW);
    }
    else {
        setCursor(IDC_HAND);
    }    
    return TRUE;
}

void ToolVideo::onPaint()
{
    render->Clear(D2D1::ColorF(0xFFFFFF));
    if (isRecording) {
        render->DrawTextLayout({ mp4Start,0.f }, timerLayout.Get(), brushIcon.Get());
        render->DrawLine({ timerEnd - 3*dpi,paddingTopBottom * 2 }, { timerEnd - 3*dpi,h - paddingTopBottom * 2 }, brushSpliter.Get(), 0.6f * dpi);
        paintIcon(timerEnd, getBtnIconLayout(L"clipboard"), hoverIndex == 6, false);
        paintIcon(timerEnd+btnSize, getBtnIconLayout(L"save"), hoverIndex == 7, false);
        paintIcon(timerEnd+2*btnSize, getBtnIconLayout(L"close"), hoverIndex == 8, false);
    }
    else {
        float borderRadius{ 4.f * dpi }, paddingTopBottom{ 4.6f * dpi };
        if (selectIndex == 0) {
            D2D1_ROUNDED_RECT rr = { { mp4Start, paddingTopBottom + marginTop, mp4End, h - paddingTopBottom }, borderRadius, borderRadius };
            render->FillRoundedRectangle(rr, brushSelect.Get());
        }
        else if (selectIndex == 1) {
            D2D1_ROUNDED_RECT rr = { { mp4End, paddingTopBottom + marginTop, gifEnd, h - paddingTopBottom }, borderRadius, borderRadius };
            render->FillRoundedRectangle(rr, brushSelect.Get());
        }
        render->DrawTextLayout({ mp4Start,0 }, formatMp4.Get(), (0 == selectIndex || hoverIndex == 0) ? brushBlue.Get() : brushIcon.Get());
        render->DrawTextLayout({ mp4End ,0 }, formatGif.Get(), (1 == selectIndex || hoverIndex == 1) ? brushBlue.Get() : brushIcon.Get());
        render->DrawLine({ gifEnd + dpi * 3,paddingTopBottom * 2 }, { gifEnd + dpi * 3,h - paddingTopBottom * 2 }, brushSpliter.Get(), 0.6f * dpi);
        paintIcon(speakerStart, getBtnIconLayout(L"speaker"), hoverIndex == 2, selectSpeaker);
        paintIcon(micStart, getBtnIconLayout(L"mic"), hoverIndex == 3, selectMic);
        render->DrawLine({ micStart + btnSize + dpi * 2,paddingTopBottom * 2 }, { micStart + btnSize + dpi * 2,h - paddingTopBottom * 2 }, brushSpliter.Get(), 0.6f * dpi);
        paintIcon(clapperStart, getBtnIconLayout(L"clapper"), hoverIndex == 4, false);
        paintIcon(closeStart, getBtnIconLayout(L"close"), hoverIndex == 5, false);
    }
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
    if (isRecording) {
        if (hoverIndex == 9) return;
        if (hoverIndex == 7) {
            saveFile();
            return;
        }
        hide();
        killTimer(100);
        auto srcPath = parent->stop();
        if (hoverIndex == 6) {
            Util::addFileToClipboard(srcPath);
        }        
        else if (hoverIndex == 8) {
            DeleteFile(srcPath.data());
        }
        close();
        parent->release();
        return;
    }
    if (hoverIndex == selectIndex) return;
    if(hoverIndex < 2)
    {
        selectIndex = hoverIndex;
        if (selectIndex == 1) {
            selectSpeaker = false;
            selectMic = false;
        }
        refresh();
        return;
    }
    if (selectIndex == 0) {
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
    }
    if (hoverIndex == 4) {
        isRecording = true;
        onTimer(10);
        resize(w - 10.f * dpi, h);
        setTimer(1000, 100);
        if (selectIndex == 0) {
            parent->startMp4(selectSpeaker, selectMic);
        }
        else if (selectIndex == 1) {
            parent->startGif();
        }
        return;
    }
    else if (hoverIndex == 5) {
        close();
        parent->release();
        return;
    }
}
void ToolVideo::onMouseMove(const int& x, const int& y)
{
    auto index{ -1 };
    if (isRecording) {
        if (x > timerEnd && x<timerEnd+btnSize) {
            index = 6;
        }
        else if (x > timerEnd + btnSize && x < timerEnd + 2 * btnSize) {
            index = 7;
        }
        else if(x > timerEnd + 2 * btnSize){
            index = 8;
        }
        else {
            index = 9;
        }
    }
    else {
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
    }
    if (index != hoverIndex) {
        hoverIndex = index;
        auto tipY{ this->y + 4 * dpi };
        if (isRecording && index == 0) {
            tipText = Lang::get(L"video.stopRecord");
            showTipAt(this->x + timerEnd + btnSize / 2, tipY);
        }
        else if (index == 0) {
            tipText = Lang::get(L"video.outputMp4");
            showTipAt(this->x + mp4Start + (mp4End - mp4Start) / 2, tipY);
        }
        else if (index == 1) {
            tipText = Lang::get(L"video.outputGif");
            showTipAt(this->x + gifEnd - (mp4End - mp4Start) / 2, tipY);
        }
        else if (index == 2) {
            tipText = Lang::get(L"video.recordSystem");
            showTipAt(this->x + speakerStart + btnSize / 2, tipY);
        }
        else if (index == 3) {
            tipText = Lang::get(L"video.recordMic");
            showTipAt(this->x + micStart + btnSize / 2, tipY);
        }
        else if (index == 4) {
            tipText = Lang::get(L"video.startRecord");
            showTipAt(this->x + clapperStart + btnSize / 2, tipY);
        }
        else if (index == 5) {
            tipText = Lang::get(L"video.exit");
            showTipAt(this->x + closeStart + btnSize / 2, tipY);
        }
        else if (index == 6) {
            tipText = Lang::get(L"video.stopClipboard");
            showTipAt(this->x + timerEnd + btnSize / 2, tipY);
        }
        else if (index == 7) {
            tipText = Lang::get(L"video.stopFile");
            showTipAt(this->x + timerEnd + btnSize / 2*3, tipY);
        }
        else if (index == 8) {
            tipText = Lang::get(L"video.stopExit");
            showTipAt(this->x + timerEnd + btnSize / 2 * 5, tipY);
        }
        else {
            hideTip();
        }
        refresh();
    }
}
void ToolVideo::onMouseLeave()
{
    hideTip();
    hoverIndex = -1;
    refresh();
}

void ToolVideo::onDpiChanged()
{
    btnSize = 32.f * dpi;
    auto fSize{ 15.f * dpi };
    btnLayout.clear();
    btnLayout.push_back(makeIconLayout(L"\ue654", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue73b", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue660", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue62d", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue650", btnSize, btnSize, fSize));
    btnLayout.push_back(makeIconLayout(L"\ue608", btnSize, btnSize, fSize));
    borderRadius *= dpi;
    paddingTopBottom *= dpi;
    fSize = 13 * dpi;
    mp4Start = 6.f * dpi;
    float formatW{ 42.f * dpi };
    formatMp4 = makeTextLayout(L"MP4", formatW, h, fSize);
    mp4End = 6.f * dpi + formatW;
    formatGif = makeTextLayout(L"GIF", formatW, h, fSize);
    gifEnd = mp4End + formatW;
    speakerStart = gifEnd + dpi * 5 + 0.6 * dpi;
    micStart = speakerStart + btnSize;
    clapperStart = micStart + btnSize + dpi * 4 + 0.6 * dpi;
    closeStart = clapperStart + btnSize;
    timerEnd = mp4Start + 116.f*dpi;
}