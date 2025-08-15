add_subdirectory(ImageDecoder)
add_subdirectory(RequestServer)
add_subdirectory(WebContent)
add_subdirectory(WebWorker)

# TODO: Enable this now that TCP Server stuff exists
# FIXME: Increase support for building targets on Windows
if (WIN32 AND ENABLE_WINDOWS_CI)
    return()
endif()

add_subdirectory(WebDriver)
