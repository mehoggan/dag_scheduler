cmake_minimum_required(VERSION 3.0.2)
project(dagtasks)

option(BUILD_SHARED_LIBS "Build shared libraries over static library." ON)
option(DAGTASKS_TESTS "Build unit tests." ON)
option(DAGTASKS_BINDINGS "Build all bindings for the native library." ON)
option(CLONE_EXTERNALS_GIT "Use git protocol while clone external deps." OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules)

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_MACOSX_RPATH 1)
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/c++11.cmake)

include_directories(${gtest_SOURCE_DIR}/include)
include_directories(${CMAKE_BINARY_DIR}/googletest-src/googletest/include)


if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-nonportable-include-path")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unknown-warning-option")
elseif(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=conversion")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=unknown-pragmas")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations")
endif()

# External Library (openssl)
set(OPENSSL_REPO "https://github.com/openssl/openssl.git" CACHE STRING
  "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
  FORCE)
if(CLONE_EXTERNALS_GIT)
  # Note openssl/openssl did not provide a git link.
  set(OPENSSL_REPO "https://github.com/openssl/openssl.git" CACHE STRING
    "Varible used to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
endif()
set(OPENSSL_ROOT "${CMAKE_BINARY_DIR}/openssl" CACHE STRING
  "Root directory for where openssl build system installs itself." FORCE)
# if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/openssl")
#   file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/openssl") 
# endif()
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/openssl")
if(NOT EXISTS "${CMAKE_CURRENT_BINARY_DIR}/openssl")
  message(FATAL_ERROR "Failed to create the install directory="
    "${CMAKE_BINARY_DIR}/openssl for openssl")
else()
  message(STATUS "Created the install directory="
    "${CMAKE_BINARY_DIR}/openssl for openssl")
endif()
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.openssl.txt.in
  ${CMAKE_BINARY_DIR}/openssl-download/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/openssl.cmake)
link_directories("${CMAKE_CURRENT_BINARY_DIR}/openssl/lib")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/openssl/include")
set(ENV{OPENSSL_ROOT_DIR} "${CMAKE_CURRENT_BINARY_DIR}/openssl")
message(STATUS "OPENSSL_ROOT_DIR = $ENV{OPENSSL_ROOT_DIR} before configure.")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/openssl/include")
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/openssl/include)
  message(FATAL_ERROR "Exiting cmake generation because openssl did not "
    "install correctly.")
endif()

# External Library (boost)
set(BOOSTBEAST_REPO "https://github.com/boostorg/boost.git" CACHE STRING
  "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
  FORCE)
if(CLONE_EXTERNALS_GIT)
  set(BOOSTBEAST_REPO "git@github.com:boostorg/boost.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
endif()
message(STATUS "Cloning boostbeast using ${BOOSTBEAST_REPO}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.boostbeast.txt.in
  ${CMAKE_BINARY_DIR}/boostbeast-download/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/boostbeast.cmake)
set(Boost_INCLUDE_DIR "${CMAKE_CURRENT_BINARY_DIR}/boostbeast/include"
  CACHE INTERNAL "Boost_INCLUDE_DIR")
if(NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/boostbeast/include)
  message(FATAL_ERROR "Exiting cmake generation because boostbeast did not "
    "install correctly.")
endif()

# External Library (yaml)
set(YAML_CPP_REPO "https://github.com/jbeder/yaml-cpp.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
if(CLONE_EXTERNALS_GIT)
  set(YAML_CPP_REPO "git@github.com:jbeder/yaml-cpp.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
endif()
message(STATUS "Cloning yaml-cpp using ${YAML_CPP_REPO}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.yaml-cpp.txt.in
        ${CMAKE_BINARY_DIR}/yaml-cpp-download/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/yaml-cpp.cmake)

# External Library (rapidjson)
set(RAPIDJSON_REPO "https://github.com/Tencent/rapidjson.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
if(CLONE_EXTERNALS_GIT)
  set(RAPIDJSON_REPO "git@github.com:Tencent/rapidjson.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
endif()
message(STATUS "Cloning rapidjson using ${RAPIDJSON_REPO}")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.rapidjson.txt.in
        ${CMAKE_BINARY_DIR}/rapidjson-download/CMakeLists.txt)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/rapidjson.cmake)
link_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/rapidjson-build")
include_directories(
  "${CMAKE_CURRENT_BINARY_DIR}/rapidjson-src/include")

include_directories(
  ${UUID_INCLUDE_DIRS}
  "${CMAKE_BINARY_DIR}/boostbeast/include"
)
link_directories(
  "${CMAKE_BINARY_DIR}/boostbeast/lib"
)
add_subdirectory(lib)

add_subdirectory(bin)

if(DAGTASKS_TESTS)
  # External Library (googletest)
  set(GOOGLETEST_REPO "https://github.com/google/googletest.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
  if(CLONE_EXTERNALS_GIT)
    set(GOOGLETEST_REPO "git@github.com:google/googletest.git" CACHE STRING
    "Varible use to determine where to clone from base od CLONE_EXTERNALS_GIT"
    FORCE)
  endif()
  message(STATUS "Cloning googletest using ${GOOGLETEST_REPO}")
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/CMakeLists.googletest.txt.in
    ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/googletest.cmake)
  add_subdirectory(tests)
endif()
