project(LogViewer)

cmake_minimum_required(VERSION 2.8.10.2)

find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(LogViewer_SOURCES
    main.cpp
    mainwindow.cpp
    Log4NetParser.cpp
    LogEntry.cpp
    LogModel.cpp
    LogModelProxy.cpp
    LogSource.cpp
    LogSourceFile.cpp
    LogViewer.cpp
    LogViewerSettingsWidget.cpp
    )

set (LogViewer_UI_SOURCES
    mainwindow.ui)

add_executable(LogViewer WIN32 ${LogViewer_SOURCES} ${LogViewer_UI_SOURCES})

QT5_USE_MODULES(LogViewer Widgets)
