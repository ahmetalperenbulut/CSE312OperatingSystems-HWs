cmake_minimum_required(VERSION 3.7)
project(OperatingSystems_HW3_1)

set(CMAKE_CXX_STANDARD 98)

set(SOURCE_FILES main.cpp 8080emu.cpp gtuos.cpp HDD.cpp MMU.cpp MMUVirtualMemory.cpp Process.cpp)
add_executable(OperatingSystems_HW3_1 ${SOURCE_FILES})