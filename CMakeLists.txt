cmake_minimum_required(VERSION 3.20)
project(NumVerse VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

file(GLOB_RECURSE APP_SOURCES src/*.cpp)
file(GLOB_RECURSE APP_HEADERS src/*.h)

add_executable(NumVerse
    ${APP_SOURCES}
    ${APP_HEADERS}
)

target_link_libraries(NumVerse PRIVATE Qt6::Widgets)
target_include_directories(NumVerse PRIVATE src)

enable_testing()
add_subdirectory(tests)
