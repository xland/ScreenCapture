#pragma once
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>
#include "TimerTask.h"

class Timer
{
public:
    ~Timer();
    static void Init();
    static Timer* Get();
    void Start(const int id,const int& timeSpan, std::function<bool()> taskFunc);
    void Remove(const int& id);
private:
    Timer();
    void asyncTask();
    std::mutex mutex;
    std::thread* t{nullptr};
    std::vector<std::shared_ptr<TimerTask>> tasks;
    bool shouldTerminate{ false };
};