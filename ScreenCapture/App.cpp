#include "pch.h"
#include "App.h"
#include "WinCap.h"

std::unique_ptr<App> app;

App::App() :dq{ winrt::Windows::System::DispatcherQueue::GetForCurrentThread() }
{
}

App::~App()
{

}

void App::init()
{
    App::initDQ();
	app = std::make_unique<App>();
    app->procCmd();
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
void App::procCmd()
{
    LPWSTR* argv;
    int argc;
    LPWSTR cmdLine = GetCommandLine();
    argv = CommandLineToArgvW(cmdLine, &argc);
    for (int i = 0; i < argc; ++i) {
        std::wstring arg{ argv[i] };
        if (arg.starts_with(L"--tool:")) {
            app->initToolBtns(std::wstring_view(arg).substr(7));
        }
    }
    LocalFree(argv);
    if (app->toolBtns.empty()) {
        app->initToolBtns(std::wstring_view(L"rect,ellipse,arrow,number,line,text,mosaic,eraser,|,undo,redo,|,pin,clipboard,save,close"));
    }
}
void App::initToolBtns(const std::wstring_view& argv)
{
    auto items = argv | std::views::split(L',');
    for (const auto& item : items) {
        std::wstring temp(item.begin(), item.end());
        toolBtns.push_back(std::move(temp));
    }
}