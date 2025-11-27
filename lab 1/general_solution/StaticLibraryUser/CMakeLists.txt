# Creates the executable target
add_executable(StaticLibraryUser main.cpp)

# Links the executable against our static library
# This copies the compiled code from the .a file into the final executable
target_link_libraries(StaticLibraryUser PRIVATE MyStaticLibrary)