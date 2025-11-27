# Creates the static library target
add_library(MyStaticLibrary STATIC
        src/MyStaticLibrary.cpp
)

# Makes the "include" directory available to other projects that link to this one
target_include_directories(MyStaticLibrary PUBLIC
        include
)