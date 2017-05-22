cmake_minimum_required(VERSION 3.6)
project(ConditQueue)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp)
add_executable(ConditQueue ${SOURCE_FILES})

TARGET_LINK_LIBRARIES( ConditQueue pthread)