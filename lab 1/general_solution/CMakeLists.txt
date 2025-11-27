cmake_minimum_required(VERSION 3.20)
project(lab_1_macOS_libraries)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(MyStaticLibrary)
add_subdirectory(StaticLibraryUser)
add_subdirectory(MyDynamicLibrary)
add_subdirectory(DynamicLibraryUser)

