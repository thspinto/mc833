cmake_minimum_required(VERSION 3.5)
project(Cmessenger)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp Client.cpp Client.h Message.cpp Message.h Group.cpp Group.h Server.cpp Server.h)
add_executable(Cmessenger ${SOURCE_FILES})