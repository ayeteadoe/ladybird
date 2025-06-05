set(SOURCES
    Lexer.cpp
    Preprocessor.cpp
)

find_package(SPIRV-Tools CONFIG REQUIRED)
find_package(SPIRV-Tools-opt CONFIG REQUIRED)
find_package(spirv_cross_core REQUIRED)
serenity_lib(LibWGSL wgsl EXPLICIT_SYMBOL_EXPORT)
target_link_libraries(LibWGSL PRIVATE AK  SPIRV-Tools-static SPIRV-Tools-opt spirv_cross_core spirv_cross_cpp)
