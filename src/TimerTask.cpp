#include "TimerTask.h"

TimerTask::TimerTask(const int& id, std::chrono::steady_clock::time_point startTime, std::function<bool()> taskFunc) 
	: id{ id }, startTime{ startTime }, taskFunc{ taskFunc } {

}