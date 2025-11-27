add_library(MyDynamicLibrary SHARED
        src/MyDynamicLibrary.cpp
)

target_compile_definitions(MyDynamicLibrary PRIVATE VECTOR_EXPORTS)

target_include_directories(MyDynamicLibrary PUBLIC
        include
)


target_link_libraries(MyDynamicLibrary PUBLIC MyStaticLibrary)