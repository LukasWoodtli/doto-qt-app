cmake_minimum_required(VERSION 3.17)

project(DotoApp)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};$ENV{HOME}/Qt/5.14.1/clang_64/lib/cmake/Qt5/")
find_package(Qt5 5.14 COMPONENTS Core REQUIRED)

