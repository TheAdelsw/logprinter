cmake_minimum_required(VERSION 3.10)   # 声明最低 CMake 版本

project(logprinter)                     # 项目名称
set(CMAKE_BUILD_TYPE Debug)     #开发中 调试


#服务端可执行文件
add_executable(logprinter_debug 
    src/main.cpp
    src/LogPrinter/core/LogPrinter.cpp
)

add_executable(client_debug
    src/client.cpp
    src/Viewer/core/Viewer.cpp
)

target_include_directories(logprinter_debug PRIVATE include)
target_include_directories(client_debug PRIVATE include)



# if(WIN32)
#     target_link_libraries(logprinter PUBLIC ws2_32)
# endif()
# Linux 不需要额外链接，socket 在 libc 里


#有元数据的cmake库(如opencv) 一般不需要target_include_directories来找声明

find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    target_compile_definitions(logprinter_debug PUBLIC LOGPRINTER_HAS_OPENCV)
    target_include_directories(logprinter_debug PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(logprinter_debug PRIVATE ${OpenCV_LIBS})

    target_compile_definitions(client_debug PUBLIC VIEWERTEST_HAS_OPENCV)
    target_include_directories(client_debug PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(client_debug PRIVATE ${OpenCV_LIBS})
    

    message(STATUS "OpenCV found: ${OpenCV_VERSION}")
else()
    message(STATUS "OpenCV NOT found — image features disabled")
endif()



#编译静态库
add_library(logprinter STATIC
    src/LogPrinter/core/LogPrinter.cpp
)

add_library(logprinter_viewer STATIC
    src/Viewer/core/Viewer.cpp
)

target_include_directories(logprinter PUBLIC include)
target_include_directories(logprinter_viewer PUBLIC include)

find_package(OpenCV REQUIRED)
target_link_libraries(logprinter PUBLIC ${OpenCV_LIBS})
target_link_libraries(logprinter_viewer PUBLIC ${OpenCV_LIBS})