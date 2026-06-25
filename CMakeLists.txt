cmake_minimum_required(VERSION 3.10)   # 声明最低 CMake 版本

project(logprinter)                     # 项目名称




add_executable(logprinter src/main.cpp)
#add_executable(logprinter src/main.cpp)




# if(WIN32)
#     target_link_libraries(logprinter PUBLIC ws2_32)
# endif()
# Linux 不需要额外链接，socket 在 libc 里




find_package(OpenCV QUIET)
if(OpenCV_FOUND)
    target_compile_definitions(logprinter PUBLIC LOGPRINTER_HAS_OPENCV)
    target_include_directories(logprinter PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(logprinter PRIVATE ${OpenCV_LIBS})
    message(STATUS "[logprinter] OpenCV found: ${OpenCV_VERSION}")
else()
    message(STATUS "[logprinter] OpenCV NOT found — image features disabled")
endif()

