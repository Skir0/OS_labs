cmake_minimum_required(VERSION 4.0)
project(lin_lab_5)

set(CMAKE_CXX_STANDARD 20)

add_executable(lin_lab_5 main.cpp)
add_executable(ProcessM processM.cpp)
add_executable(ProcessA processA.cpp)
add_executable(ProcessP processP.cpp)
add_executable(ProcessS processS.cpp)
add_dependencies(lin_lab_5 ProcessM ProcessA ProcessP ProcessS)