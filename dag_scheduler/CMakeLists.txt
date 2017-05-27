cmake_minimum_required(VERSION 3.0.2)
project(u_dagtasks)

option(BUILD_SHARED_LIBS "Build shared libraries over static library." ON)
option(U_DAGTASKS_TESTS "Build unit tests." ON)
option(U_DAGTASKS_BINDINGS "Build all bindings for the native library." ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 1)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/c++11.cmake)

add_subdirectory(lib)

if(U_DAGTASKS_TESTS)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.googletest.txt.in
    ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gtest.cmake)
  add_subdirectory(tests)
endif()
