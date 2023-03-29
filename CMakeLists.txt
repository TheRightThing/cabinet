cmake_minimum_required(VERSION 2.6)
project(cabinet)

set(CMAKE_C_FLAGS "-std=c11 -Werror -Wall -Wextra -Wfatal-errors -g")

file(GLOB_RECURSE cabinet_src ./src/*.c)

include_directories(build/ncurses/include)
add_executable(cabinet ${cabinet_src} ./build/ncurses/lib/libncurses.a)