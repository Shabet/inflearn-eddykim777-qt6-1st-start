cmake_minimum_required(VERSION 3.19)
project(P01_QMainWindow LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

set(P01_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
)
qt_add_resources(P01_SOURCES res.qrc)
qt_add_executable(P01_QMainWindow
    WIN32 MACOSX_BUNDLE
    ${P01_SOURCES}
)

target_link_libraries(P01_QMainWindow
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS P01_QMainWindow
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET P01_QMainWindow
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
