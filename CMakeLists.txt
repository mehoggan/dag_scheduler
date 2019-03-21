cmake_minimum_required(VERSION 3.0.2)
project(dagtasks)

option(BUILD_SHARED_LIBS "Build shared libraries over static library." ON)
option(DAGTASKS_TESTS "Build unit tests." ON)
option(DAGTASKS_BINDINGS "Build all bindings for the native library." ON)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 1)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/c++11.cmake)

include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.boostbeast.txt.in
  ${CMAKE_BINARY_DIR}/boostbeast-download/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/boostbeast.cmake)

include_directories(
  ${UUID_INCLUDE_DIRS}
  "${CMAKE_BINARY_DIR}/boostbeast/include"
)
link_directories(
  "${CMAKE_BINARY_DIR}/boostbeast/lib"
)
add_subdirectory(lib)

if(DAGTASKS_TESTS)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.googletest.txt.in
    ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/googletest.cmake)
  add_subdirectory(tests)
endif()
