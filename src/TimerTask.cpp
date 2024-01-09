#include "TimerTask.h"

TimerTask::TimerTask(const int& id, std::chrono::steady_clock::time_point startTime, std::function<bool()> taskFunc) 
	: Id{ id }, StartTime{ startTime }, TaskFunc{ taskFunc } {

}