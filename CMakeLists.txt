cmake_minimum_required(VERSION 3.13)
project(sm_ipc)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads)

include_directories(include)


add_definitions(-DSK_OS_LINUX)

add_subdirectory(src/sk_os )
add_subdirectory(src/sk_sm)

add_executable(sm_ipc main.cpp)
target_link_libraries(sm_ipc sk_os)