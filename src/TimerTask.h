#pragma once
#include <chrono>
#include <functional>

class TimerTask
{
public:
    TimerTask(const int& id, std::chrono::steady_clock::time_point startTime, std::function<void()> taskFunc);
    int id;
    std::chrono::steady_clock::time_point startTime;
    std::function<void()> taskFunc;
};