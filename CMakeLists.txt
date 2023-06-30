cmake_minimum_required(VERSION 3.25)
project(proj_tifo)

set(CMAKE_CXX_STANDARD 23)

include_directories(/opt/homebrew/Cellar/libpng/1.6.40/include)

link_directories(/opt/homebrew/Cellar/libpng/1.6.40/lib)

add_executable(proj_tifo main.cpp image/image.cc image/image.hh)

target_link_libraries(proj_tifo png)
