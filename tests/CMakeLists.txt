
cmake_minimum_required(VERSION 2.6)

project(backlot-tests)

include_directories(../include include/support ${LUA_INCLUDE_DIR})

add_executable(buffertest ../src/Buffer.cpp buffertest.cpp)
add_executable(referencecounting referencecounting.cpp)
