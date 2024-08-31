cmake_minimum_required(VERSION 3.14)

if (APPLE)
  foreach (HOMEBREW_PKG openssl sqlite)
    execute_process(COMMAND brew --prefix ${HOMEBREW_PKG} OUTPUT_VARIABLE HOMEBREW_PREFIX OUTPUT_STRIP_TRAILING_WHITESPACE)
    list(APPEND CMAKE_PREFIX_PATH "${HOMEBREW_PREFIX}")
  endforeach ()
endif ()

project(signalbackup-tools)

find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

set(CMAKE_CXX_EXTENSIONS off)
if (CMAKE_VERSION VERSION_LESS "3.30")  # the CMAKE_CXX_STANDARD_LATEST variable was only introduced in 3.30
  set(CMAKE_CXX_STANDARD 20)
elseif(CMAKE_CXX_STANDARD_LATEST) # if cmake version is ok and the variable is set, use it
  set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD_LATEST})
endif()
set(CMAKE_CXX_STANDARD_REQUIRED on)

# find and set DBUS include and library paths
if (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  if (WITHOUT_DBUS)
    add_definitions(-DWITHOUT_DBUS)
  else()  
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(DBUS REQUIRED dbus-1)
    include_directories(${DBUS_INCLUDE_DIRS})
    set(DBUS_LIBS_ABSOLUTE)
    foreach(lib ${DBUS_LIBRARIES})
      set(tmp DBUS_${lib}_ABS)
      find_library(${tmp} ${lib} ${DBUS_LIBRARY_DIR})
      list(APPEND DBUS_LIBS_ABSOLUTE ${${tmp}})
    endforeach()
  endif()
endif()

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

target_link_libraries(signalbackup-tools OpenSSL::Crypto SQLite::SQLite3 ${SECLIB} ${CFLIB} ${DBUS_LIBS_ABSOLUTE})
