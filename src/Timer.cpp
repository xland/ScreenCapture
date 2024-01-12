#include "Timer.h"
#include <future>
#include <thread>

Timer* timer;

Timer::Timer()
{
    t = new std::thread(&Timer::asyncTask, this);
}

Timer::~Timer()
{
    shouldTerminate = true;
    if (t->joinable())
    {
        t->join();
    }
    delete t;
}

void Timer::Init()
{
    timer = new Timer();
}

Timer* Timer::Get()
{
    return timer;
}

void Timer::Start(const int id, const int& timeSpan, std::function<bool()> taskFunc)
{
    auto it = std::find_if(tasks.begin(), tasks.end(), [id](auto& task) {
        return task->Id == id;
        });
    auto startTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeSpan);
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (it != tasks.end()) {
            it->get()->StartTime = startTime;
        }
        else {
            tasks.push_back(std::make_shared<TimerTask>(id, startTime, taskFunc));
        }
        std::sort(tasks.begin(), tasks.end(), [](auto& a, auto& b) {
            return a->StartTime < b->StartTime;
            });
    }
}

void Timer::Remove(const int& id)
{
    std::lock_guard<std::mutex> lock(mutex);
    std::erase_if(tasks, [&id](std::shared_ptr<TimerTask> task) {
        return task->Id == id;
        });
}

void Timer::asyncTask()
{
    while (!shouldTerminate)
    {
        auto now = std::chrono::steady_clock::now();        
        auto currentTime = std::chrono::steady_clock::now();
        auto it = std::remove_if(tasks.begin(), tasks.end(), [currentTime](auto& task) {
            if (task->StartTime <= currentTime) {
                return task->TaskFunc();
            }
            return false;
            });
        if (it != tasks.end()) {
            std::lock_guard<std::mutex> lock(mutex);
            if (!shouldTerminate) {
                tasks.erase(it, tasks.end());
            }
        }
        //{
        //    std::lock_guard<std::mutex> lock(mutex);
        //    std::erase_if(tasks, [&currentTime](auto& task) {
        //        if (task->StartTime <= currentTime) {
        //            return task->TaskFunc();
        //        }
        //        return false;
        //        });
        //}
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}