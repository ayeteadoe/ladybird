add_executable(ladybird
    EventLoopImplementationWin32.cpp
    main.cpp
)
lagom_windows_bin(ladybird CONSOLE)
create_ladybird_bundle(ladybird)
