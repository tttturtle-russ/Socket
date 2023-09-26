cmake_minimum_required(VERSION 3.27)
project(Socket)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/Include)

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs)

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

add_executable(Socket main.cpp
        mainwindow.cpp
        download.cpp
        upload.cpp
        log/log.cpp
        log/log.h
        Thread.cpp
        Thread.h
        Sender.cpp
)
target_link_libraries(Socket
        Qt::Core
        Qt::Gui
        Qt::Widgets
)

