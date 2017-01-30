project(LogViewer)

cmake_minimum_required(VERSION 2.8.10.2)

find_package(Qt5Widgets REQUIRED)

enable_testing()

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(LogViewer_SOURCES
    Log4NetParser.cpp
    LogEntry.cpp
    LogModel.cpp
    LogModelProxy.cpp
    LogSource.cpp
    LogSourceFile.cpp
    )

set (LogViewer_UI_SOURCES
    LogViewer.cpp
    LogViewerSettingsWidget.cpp
    mainwindow.cpp
    mainwindow.ui)

add_library(LogLib SHARED ${LogViewer_SOURCES})
QT5_USE_MODULES(LogLib Core)
QT5_USE_MODULES(LogLib Gui)

add_executable(LogViewer WIN32 main.cpp ${LogViewer_UI_SOURCES})
target_link_libraries(LogViewer LogLib)
QT5_USE_MODULES(LogViewer Widgets)

###
set(testfiles
    tests/LogSourceTest.cpp
    )

foreach(testfile ${testfiles})
    get_filename_component(testname ${testfile} NAME_WE)
    add_executable(${testname} ${testfile})
    target_link_libraries(${testname} LogLib)
    QT5_USE_MODULES(${testname} Test)
    add_test(${testname} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${testname})
endforeach(testfile)

