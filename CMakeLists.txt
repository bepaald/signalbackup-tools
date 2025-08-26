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

# For GCC:
# The version here is a bit pessimistic. on compiler-explorer the warning is gone
# at the first gcc-9 release, but I have reports (and verified in VM) the warning
# appears with "gcc-9.4-ubuntu" (on Mint 20.3). At worst here, we are suppressing
# a non-existing warning on newer compilers
# For CLANG:
# This is just based on godbolt testing...
if ((CMAKE_COMPILER_IS_GNUCC AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 10)
    OR
    (CMAKE_COMPILER_IS_CLANG AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 12))
  add_compile_options(-Wno-attributes)
endif()

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

file(GLOB SOURCES CONFIGURE_DEPENDS */*.cc */*.h *.cc *.h)
add_executable(signalbackup-tools ${SOURCES})

if (WIN32)
  target_compile_definitions(signalbackup-tools PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
  target_link_libraries(signalbackup-tools PRIVATE crypt32)
  # disable some warnings on windows (int conversions and use of localtime vs localtime_s)
  target_compile_options(signalbackup-tools PUBLIC /utf-8 /wd4244 /wd4267 /wd4996)
endif()

target_link_libraries(signalbackup-tools PRIVATE OpenSSL::Crypto PRIVATE SQLite::SQLite3 ${SECLIB} ${CFLIB} ${DBUS_LIBS_ABSOLUTE})
