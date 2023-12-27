#include "Timer.h"
#include <future>
#include <thread>

Timer* timer;

Timer::Timer()
{
    
}

Timer::~Timer()
{
    shouldTerminate = true;
    if (t->joinable())
    {
        t->join();
    }
    delete t;
    delete timer;
}

void Timer::init()
{
    timer = new Timer();
}

Timer* Timer::get()
{
    return timer;
}

void Timer::Start(const int id, const int& timeSpan, std::function<void()> taskFunc)
{
    if (t == nullptr) {
        t = new std::thread(&Timer::asyncTask, this);
    }
    auto it = std::find_if(tasks.begin(), tasks.end(), [id](auto& task) {
        return task->id == id;
        });
    auto startTime = std::chrono::steady_clock::now() + std::chrono::milliseconds(timeSpan);
    {
        std::lock_guard<std::mutex> lock(mutex);
        if (it != tasks.end()) {
            it->get()->startTime = startTime;
        }
        else {
            tasks.push_back(std::make_shared<TimerTask>(id, startTime, taskFunc));
        }
        std::sort(tasks.begin(), tasks.end(), [](auto& a, auto& b) {
            return a->startTime < b->startTime;
            });
    }
}

void Timer::Remove(const int& id)
{
    auto it = std::find_if(tasks.begin(), tasks.end(), [id](std::shared_ptr<TimerTask> task){
        return task->id == id;
    });
    if (it != tasks.end()) {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.erase(it);
    }
}

void Timer::asyncTask()
{
    while (!shouldTerminate)
    {
        auto now = std::chrono::steady_clock::now();        
        auto currentTime = std::chrono::steady_clock::now();
        auto it = std::remove_if(tasks.begin(), tasks.end(), [currentTime](auto& task) {
            if (task->startTime <= currentTime) {
                task->taskFunc();
                return true;
            }
            return false;
            });
        if (it != tasks.end()) {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.erase(it, tasks.end());
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}