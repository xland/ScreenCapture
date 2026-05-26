#include "pch.h"
#include "App.h"
#include "Win/WinCap.h"

std::unique_ptr<App> app;


App::App(HINSTANCE hInstance) :dq{ winrt::Windows::System::DispatcherQueue::GetForCurrentThread() }, hInstance(hInstance)
{
}

App::~App()
{

}

void App::init(HINSTANCE hInstance)
{
    App::initDQ();
	app = std::make_unique<App>(hInstance);
    WinCap::init();

}

App* App::get()
{
    return app.get();
}
void App::exit(const int& code)
{
    PostQuitMessage(code);
}
void App::initDQ()
{
    auto optSize = sizeof(DispatcherQueueOptions);
    DispatcherQueueOptions options{ optSize,DQTYPE_THREAD_CURRENT,DQTAT_COM_NONE };
    static winrt::Windows::System::DispatcherQueueController controller{ nullptr };
    auto dqc = winrt::put_abi(controller);
    auto hr = CreateDispatcherQueueController(options, reinterpret_cast<ABI::Windows::System::IDispatcherQueueController**>(dqc));
    if (FAILED(hr))
    {
        MessageBox(NULL, L"无法创建DispatcherQueueController", L"系统提示", MB_OK);
        ExitProcess(-1);
    }
}