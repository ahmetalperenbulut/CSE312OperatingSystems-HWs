cmake_minimum_required(VERSION 3.7)
project(OperatingSystems_HW2)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp 8080emu.cpp gtuos.cpp MMU.cpp process.cpp)
add_executable(OperatingSystems_HW2 ${SOURCE_FILES})