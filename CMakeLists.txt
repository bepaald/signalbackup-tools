cmake_minimum_required(VERSION 3.14)

set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_STANDARD 20)
#if version > 3.30 && CMAKE_CXX_STANDARD_LATEST is set
#set standard to CMAKE_CXX_STANDARD_LATEST (if set)
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

if (APPLE)
  find_library(SECLIB Security)
  if (NOT SECLIB)
    message(FATAL_ERROR "Failed to find required framework 'Security'")
  endif()
  find_library(CFLIB CoreFoundation)
  if (NOT CFLIB)
    message(FATAL_ERROR "Failed to find required framework 'CoreFoundation'")
  endif()
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS *.cc *.h)
add_executable(signalbackup-tools ${SOURCES})

target_link_libraries(signalbackup-tools OpenSSL::Crypto SQLite::SQLite3 ${SECLIB} ${CFLIB})
