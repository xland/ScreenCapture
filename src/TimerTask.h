#pragma once
#include <chrono>
#include <functional>

class TimerTask
{
public:
    TimerTask(const int& id, std::chrono::steady_clock::time_point startTime, std::function<bool()> taskFunc);
    int Id;
    std::chrono::steady_clock::time_point StartTime;
    std::function<bool()> TaskFunc;
};