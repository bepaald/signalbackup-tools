cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

project(signalbackup-tools)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cc *.h)
add_executable(signalbackup-tools ${SOURCES})

target_link_libraries(signalbackup-tools OpenSSL::SSL SQLite::SQLite3)
