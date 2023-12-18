#pragma once
#include <chrono>
#include <thread>
#include <functional>
class Timeout
{
public:
    Timeout(std::function<void()> callBack);
    ~Timeout();
    void Start();

private:
    void asyncTask();
    std::atomic<bool> shouldTerminate{false};
    std::function<void()> callBack;
    std::chrono::steady_clock::time_point startTime;
    std::thread t;
};