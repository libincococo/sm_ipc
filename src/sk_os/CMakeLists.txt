cmake_minimum_required(VERSION 3.13)
project(sk_os)

set(CMAKE_CXX_STANDARD 11)

find_package(Threads)
aux_source_directory(. DIR_LIBS_SRC)
add_library(sk_os SHARED ${DIR_LIBS_SRC})
target_link_libraries(sk_os pthread)
