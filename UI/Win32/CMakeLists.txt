add_executable(ladybird
    Application.cpp
    EventLoopImplementationWin32.cpp
    main.cpp
)
target_link_libraries(ladybird PRIVATE Comctl32)
lagom_windows_bin(ladybird CONSOLE)
create_ladybird_bundle(ladybird)
