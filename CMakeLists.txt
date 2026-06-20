cmake_minimum_required(VERSION 3.16)
project(TinyBar)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network DBus Svg SvgWidgets Test)
find_package(LayerShellQt REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_library(TinyBarCore
    src/BarManager.cpp
    src/BarManager.h
    src/Clock.cpp
    src/Clock.h
    src/BatteryOld.cpp
    src/BatteryOld.h
    src/HyprListener.cpp
    src/HyprListener.h
    src/DBusObject.cpp
    src/DBusObject.h
    src/Settings.cpp
    src/Settings.h
    src/Battery.cpp
    src/Battery.h
)

target_link_libraries(TinyBarCore
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Network
        Qt6::DBus
        Qt6::Svg
        Qt6::SvgWidgets
        LayerShellQt::Interface
)

qt_add_resources(TinyBarCore
    PREFIX "/"
    FILES
        resources/battery-empty.svg
        resources/battery-full.svg
        resources/battery-half.svg
        resources/battery-quarter.svg
        resources/battery-three-quarters.svg
        resources/bolt.svg
        resources/plug-circle-bolt.svg
        resources/settings-gear.svg
)

add_executable(TinyBar src/main.cpp)

target_link_libraries(TinyBar
    PRIVATE
        TinyBarCore
)

add_executable(TestBattery tests/TestBattery.cpp)

target_link_libraries(TestBattery PRIVATE TinyBarCore Qt6::Test)
add_test(NAME TestBattery COMMAND TestBattery)
