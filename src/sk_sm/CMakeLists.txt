cmake_minimum_required(VERSION 3.13)
project(sk_sm)

set(CMAKE_CXX_STANDARD 11)

aux_source_directory(. DIR_LIBS_SRC)
add_library(sk_sm SHARED ${DIR_LIBS_SRC})
target_link_libraries(sk_sm sk_os)