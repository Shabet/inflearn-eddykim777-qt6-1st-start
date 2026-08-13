cmake_minimum_required(VERSION 3.19)
project(P01_Console_Application LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core)

qt_standard_project_setup()

qt_add_executable(P01_Console_Application
    main.cpp
)

target_link_libraries(P01_Console_Application
    PRIVATE
        Qt::Core
)

install(TARGETS P01_Console_Application
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
