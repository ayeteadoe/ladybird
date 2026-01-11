# Relevant context/information for consuming WindowsAppSDK outside in a non-MSBuild project with ClangCL:
#   - https://github.com/microsoft/WindowsAppSDK/discussions/1891
#       - https://learn.microsoft.com/en-us/windows/uwp/cpp-and-winrt-apis/intro-to-using-cpp-with-winrt#visual-studio-support-for-cwinrt-xaml-the-vsix-extension-and-the-nuget-package
#   - https://github.com/microsoft/WindowsAppSDK/issues/3901
#       - https://github.com/microsoft/WindowsAppSDK-Samples/tree/release/experimental/Samples/WindowsAIFoundry/cpp-console-sparse/vcpkg_ports/windowsappsdk
#       - https://github.com/res2k/winui3_cmake

# TODO: See the GTK4 draft for how we'll want to only conditionally build/use the ports when this UI framework is specified

find_package(windowsappsdk CONFIG REQUIRED)
get_target_property(CPPWINRT Windows::CppWinRT IMPORTED_LOCATION)
find_program(MIDL NAMES midl.exe REQUIRED)

function(generate_cppwinrt_sources CPPWINRT_SOURCES)
    set(generate_cppwinrt_files "")
    foreach(cppwinrt_basename ${ARGN})
        set(cppwinrt_base_file_path "${CMAKE_CURRENT_SOURCE_DIR}/${cppwinrt_basename}")
        set(cppwinrt_idl_file_path "${cppwinrt_base_file_path}.idl")
        add_custom_command(
            OUTPUT ${cppwinrt_winmd_file_path}
            # TODO: commands, something like midl.exe /metadata_dir "C:\Program Files (x86)\Windows Kits\10\UnionMetadata\10.0.26100.0" /winrt /winmd App.winmd App.idl
            #       but we also need to support generated content from the App.xaml file
            COMMAND ${MIDL} 
            DEPENDS ${cppwinrt_idl_file_path}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        )
        # TODO: Generated headers/sources with .winmd file and append them to `generate_cppwinrt_files` CMake var
    endforeach()
    set(${CPPWINRT_SOURCES} "${generate_cppwinrt_files}" PARENT_SCOPE)
endfunction()

add_executable(ladybird
    main.cpp
)
generate_cppwinrt_sources(CPPWINRT_SOURCES
    App
)
target_sources(ladybird PRIVATE ${CPPWINRT_SOURCES})
target_link_libraries(ladybird PRIVATE WindowsAppSdk::Bootstrap WindowsAppSdk::Runtime)
lagom_windows_bin(ladybird CONSOLE)
create_ladybird_bundle(ladybird)
