# cmake version to be used
cmake_minimum_required( VERSION 3.0 )

# project name
project( filesystem )

# include
include_directories( ./include ./src )

# target
add_executable( binary ./src/main.cpp ./src/disk.cpp ./src/file.cpp )
