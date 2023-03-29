cmake_minimum_required(VERSION 2.6)
project(cabinet)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_C_FLAGS "-std=c11 -Werror -Wall -Wextra -Wfatal-errors -g")

file(GLOB_RECURSE cabinet_src ./src/*.c)

add_library(ncurses IMPORTED STATIC)
set_property(TARGET ncurses PROPERTY IMPORTED_LOCATION "build/ncurses/lib/libncurses.a")

include_directories(./build/ncurses/include)
add_executable(cabinet ${cabinet_src})
target_link_libraries(cabinet ncurses)

file(GLOB_RECURSE cabinet_test_src ./test/*.c)
list(FILTER cabinet_src EXCLUDE REGEX "main.c")
add_executable(cabinet_test ${cabinet_test_src} ${cabinet_src})
target_link_libraries(cabinet_test ncurses)

add_custom_command(TARGET cabinet_test POST_BUILD
	COMMAND cabinet_test
	COMMENT "Testing cabinet...")