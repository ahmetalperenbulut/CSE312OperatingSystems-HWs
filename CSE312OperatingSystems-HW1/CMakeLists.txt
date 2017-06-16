cmake_minimum_required(VERSION 3.7)
project(OperatingSystems_HW1)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp gtuos.cpp 8080emu.cpp)
add_executable(OperatingSystems_HW1 ${SOURCE_FILES})