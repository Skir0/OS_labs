cmake_minimum_required(VERSION 3.10)
project(ProcessManager)

set(CMAKE_CXX_STANDARD 11)

add_executable(user user.cpp)
add_executable(killer Killer.cpp)