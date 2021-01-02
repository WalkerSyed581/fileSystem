# cmake version to be used
cmake_minimum_required( VERSION 3.0 )

# project name
project( filesystem )

# include
include_directories( ./include ./src )

find_package (Threads)

# target
add_executable( client ./src/client.cpp )

add_executable( server ./src/server.cpp ./src/disk.cpp ./src/file.cpp )


target_link_libraries (server ${CMAKE_THREAD_LIBS_INIT})
