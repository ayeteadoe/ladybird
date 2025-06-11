set(TEST_SOURCES
    TestColor.cpp
    TestImageWriter.cpp
    TestQuad.cpp
    TestRect.cpp
)

# FIXME: Add support for these tests that load content from the test-inputs folder on Windows
if(NOT WIN32)
    list(APPEND TEST_SOURCES
        BenchmarkJPEGLoader.cpp
        TestImageDecoder.cpp
        TestWOFF.cpp
        TestWOFF2.cpp
    )
endif()

foreach(source IN LISTS TEST_SOURCES)
    serenity_test("${source}" LibGfx LIBS LibGfx)
endforeach()
