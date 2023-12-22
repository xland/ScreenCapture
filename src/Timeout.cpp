#include "Timeout.h"
#include <future>
#include <thread>


Timeout::Timeout()
{
    t = std::thread(&Timeout::asyncTask, this);
}

Timeout::~Timeout()
{
    shouldTerminate = true;
    if (t.joinable())
    {
        t.join();
    }
}

void Timeout::Start(const int id, const int& timeSpan, std::function<bool(const int&)> taskFunc)
{
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
            tasks.push_back(std::make_shared<Task>(id, startTime, taskFunc));
        }
        std::sort(tasks.begin(), tasks.end(), [](auto& a, auto& b) {
            return a->startTime < b->startTime;
            });
    }
}

void Timeout::Remove(const int& id)
{
    auto it = std::find_if(tasks.begin(), tasks.end(), [id](std::shared_ptr<Task> task){
        return task->id == id;
    });
    if (it != tasks.end()) {
        std::lock_guard<std::mutex> lock(mutex);
        tasks.erase(it);
    }
}

void Timeout::asyncTask()
{
    while (!shouldTerminate)
    {
        auto now = std::chrono::steady_clock::now();        
        auto currentTime = std::chrono::steady_clock::now();
        auto it = std::remove_if(tasks.begin(), tasks.end(), [currentTime](auto& task) {
            if (task->startTime <= currentTime) {
                return task->taskFunc(task->id);
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