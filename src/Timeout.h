#pragma once
#include <chrono>
#include <thread>
#include <functional>
#include <mutex>

class Task
{
public:
    Task(const int& id, std::chrono::steady_clock::time_point startTime, std::function<void(const int&)> taskFunc) 
        :id{ id }, startTime{startTime},taskFunc{taskFunc}{}
    int id;
    std::chrono::steady_clock::time_point startTime;
    std::function<void(const int&)> taskFunc;
};

class Timeout
{
public:
    Timeout();
    ~Timeout();
    void Start(const int id,const int& timeSpan, std::function<void(const int&)> taskFunc);

private:
    void asyncTask();
    std::mutex mutex;
    std::thread t;
    std::vector<std::shared_ptr<Task>> tasks;
    bool shouldTerminate{ false };
};