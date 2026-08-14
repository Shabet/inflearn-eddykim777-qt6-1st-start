cmake_minimum_required(VERSION 3.19)
project(P02_ModelView LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(P02_ModelView
    WIN32 MACOSX_BUNDLE
    main.cpp
    widget.cpp
    widget.h
)

target_link_libraries(P02_ModelView
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS P02_ModelView
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET P02_ModelView
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
