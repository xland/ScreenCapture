#include "Recorder.h"

Recorder::Recorder()
{
}

Recorder::~Recorder()
{
}

void Recorder::Init()
{
    listenLeftBtnDown(std::bind(&Recorder::OnLeftBtnDown, this, std::placeholders::_1, std::placeholders::_2));
    listenMouseMove(std::bind(&Recorder::OnMouseMove, this, std::placeholders::_1, std::placeholders::_2));
    listenCustomMsg(std::bind(&Recorder::OnCustomMsg, this, std::placeholders::_1, std::placeholders::_2));
    listenPaint(std::bind(&Recorder::OnPaint, this, std::placeholders::_1));
}

void Recorder::OnLeftBtnDown(const int& x, const int& y)
{
}

void Recorder::OnCustomMsg(const EventType& type, const uint32_t& msg)
{
}

void Recorder::OnMouseMove(const int& x, const int& y)
{
}

void Recorder::OnPaint(SkCanvas* canvas)
{
}
