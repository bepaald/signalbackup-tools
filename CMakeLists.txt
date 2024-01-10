cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED on)

if (APPLE)
  foreach (HOMEBREW_PKG openssl sqlite)
    execute_process(COMMAND brew --prefix ${HOMEBREW_PKG} OUTPUT_VARIABLE HOMEBREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}")
  endforeach ()
endif ()

project(signalbackup-tools)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cc *.h)
add_executable(signalbackup-tools ${SOURCES})

target_link_libraries(signalbackup-tools OpenSSL::Crypto SQLite::SQLite3)
