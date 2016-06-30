cmake_minimum_required(VERSION 3.5)
project(CmessengerServer)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SERVER_SOURCE_FILES serverMain.cpp User.cpp User.h Message.cpp Message.h Group.cpp Group.h Server.cpp Server.h md5.cpp md5.h)
add_executable(CmessengerServer ${SERVER_SOURCE_FILES})


project(CmessengerClient)
set(CLIENT_SOURCE_FILES clientMain.cpp Client.cpp Client.h md5.cpp md5.h)
add_executable(CmessengerClient ${CLIENT_SOURCE_FILES})
