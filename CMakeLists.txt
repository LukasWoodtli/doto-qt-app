cmake_minimum_required(VERSION 3.16)

project(DotoApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd-party)
include(3rdparty)


set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};$ENV{HOME}/Qt/5.14.1/clang_64/lib/cmake/Qt5/")
find_package(Qt5 5.14 COMPONENTS Core Test Sql REQUIRED)

set(CMAKE_AUTOMOC ON)

enable_testing()


add_subdirectory(db)



add_subdirectory(integration)

