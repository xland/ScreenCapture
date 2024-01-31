 ```c++
 HANDLE mutex = CreateMutex(NULL, TRUE, L"ScreenCapture");
 if (mutex == NULL)
 {
     MessageBox(NULL, L"Failed to create process mutex.", L"Error", MB_OK | MB_ICONERROR);
     return -1;
 }
 if (GetLastError() == ERROR_ALREADY_EXISTS)
 {
     CloseHandle(mutex);
     return -1;
 }
 HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE);
 if (FAILED(hr))
 {
     MessageBox(NULL, L"Failed to initialize COM library.", L"Error", MB_OK | MB_ICONERROR);
     return -1;
 }
 SkGraphics::Init();
 App::Init();
 MSG msg;
 while (GetMessage(&msg, NULL, 0, 0))
 {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
 }
 App::Dispose();
 CoUninitialize();
 CloseHandle(mutex);
 auto code = App::GetExitCode();
 return code;
```