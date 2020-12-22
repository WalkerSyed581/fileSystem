# cmake version to be used
cmake_minimum_required( VERSION 3.0 )

# project name
project( filesystem )

# include
include_directories( ./include ./src )

find_package (Threads)

# target
add_executable( binary ./src/main.cpp ./src/disk.cpp ./src/file.cpp )

target_link_libraries (binary ${CMAKE_THREAD_LIBS_INIT})
