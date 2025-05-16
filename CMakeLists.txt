cmake_minimum_required(VERSION 3.10)
project(ScreenCapture LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt设置
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 构建类型设置
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

# 配置特定设置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_definitions(DEBUG)
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release" AND MSVC)
    # 对应于MultiThreaded设置
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded")
endif()


# 设置静态链接运行时库
if(MSVC)
    # MSVC 静态链接 C/C++ 运行时
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    # 禁用自动链接
    add_definitions(-DWIN32_LEAN_AND_MEAN)
else()
    # GNU 编译器 (GCC, MinGW) 的静态链接设置
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -static-libgcc -static-libstdc++")
    # 对于 MinGW，可能需要下面这行来处理一些链接问题
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif()

# 使用文件查找函数自动收集源文件
file(GLOB_RECURSE PROJECT_SOURCES
    "App/*.cpp"
    "Shape/*.cpp"
    "Tool/*.cpp"
    "Win/*.cpp"
    "main.cpp"
)

# 收集头文件 (.h)
file(GLOB_RECURSE PROJECT_HEADERS 
    "App/*.h"
    "Shape/*.h"
    "Tool/*.h"
    "Win/*.h"
)

# 确保每次构建都检查文件列表
set_property(DIRECTORY PROPERTY CMAKE_CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/App"
    "${CMAKE_CURRENT_SOURCE_DIR}/Shape"
    "${CMAKE_CURRENT_SOURCE_DIR}/Tool"
    "${CMAKE_CURRENT_SOURCE_DIR}/Win"
)

# 添加资源
set(RESOURCES
    res.qrc
)

# 设置Qt静态库路径
if(NOT DEFINED QT_PATH)
    if(MSVC)
        set(QT_PATH $ENV{QT_STATIC_MSVC_DIR})
    else()
        set(QT_PATH $ENV{QT_STATIC_MINGW_DIR})
    endif()
endif()

set(CMAKE_PREFIX_PATH ${QT_PATH}/lib/cmake)
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Concurrent)

# 特定于平台的依赖库
if(WIN32)
    set(PLATFORM_LIBS 
        dwmapi.lib
        shcore.lib
    )
    
    # 静态Qt在Windows上需要的额外依赖库
    if(MSVC)
        list(APPEND PLATFORM_LIBS
            Ws2_32.lib
            Winmm.lib
            Imm32.lib
            Dwrite.lib
            D3d11.lib
            Iphlpapi.lib
            Crypt32.lib
            Ole32.lib
            Netapi32.lib
            Userenv.lib
            Version.lib
            UxTheme.lib
            Dwmapi.lib
            Setupapi.lib
        )
    else()
        # MinGW链接库名称与MSVC不同
        list(APPEND PLATFORM_LIBS
            -lws2_32
            -lwinmm
            -limm32
            -ldwrite
            -ld3d11
            -liphlpapi
            -lcrypt32
            -lole32
            -lnetapi32
            -luserenv
            -lversion
            -luxtheme
            -ldwmapi
            -lsetupapi
        )
    endif()
endif()

# 创建可执行文件
add_executable(${PROJECT_NAME} WIN32 
    ${SOURCES} 
    ${HEADERS} 
    ${MOC_HEADERS} 
    ${RESOURCES}
    App/Res.rc
    Res/logo.ico
    Res/iconfont.ttf
)

# 设置包含目录
target_include_directories(${PROJECT_NAME} PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Lib
)

# 链接Qt和平台库
target_link_libraries(${PROJECT_NAME} PRIVATE 
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
    ${PLATFORM_LIBS}
)

# 设置输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)

# 调试版本特定设置
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(${PROJECT_NAME} PRIVATE DEBUG)
endif()
