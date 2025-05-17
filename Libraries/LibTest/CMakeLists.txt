add_library(LibTestMain OBJECT TestMain.cpp AssertionHandler.cpp)

target_link_libraries(LibTestMain PUBLIC GenericClangPlugin)

if (ENABLE_WEB_TARGETS)
    add_library(JavaScriptTestRunnerMain OBJECT JavaScriptTestRunnerMain.cpp)
endif()

set(SOURCES
    TestSuite.cpp
)

add_library(LibTest ${SOURCES})
lagom_generate_export_header(LibTest test)
target_link_libraries(LibTest PRIVATE AK LibCore LibFileSystem)
set_target_properties(LibTest PROPERTIES OUTPUT_NAME lagom-test)
