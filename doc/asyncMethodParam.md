```c++
    auto func = std::bind(&Recorder::hideDragger, this, std::placeholders::_1);
    Timer::get()->Start(id, 800, func);
```

```c++
std::function<bool(const int&)> taskFunc
taskFunc(id)
```