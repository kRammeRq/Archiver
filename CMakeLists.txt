cmake_minimum_required(VERSION 3.10)
project(MyArchiver)

set(CMAKE_CXX_STANDARD 17)

find_package(ZLIB REQUIRED)

add_executable(MyArchiver main.cpp)

target_link_libraries(MyArchiver ZLIB::ZLIB)