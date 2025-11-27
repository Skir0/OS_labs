add_executable(DynamicLibraryUser main.cpp)

# Связываем с MyDynamicLibrary. Поскольку она зависит от MyStaticLibrary,
# CMake автоматически свяжет и ее.
target_link_libraries(DynamicLibraryUser PRIVATE MyDynamicLibrary)