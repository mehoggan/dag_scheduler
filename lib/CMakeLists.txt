cmake_minimum_required(VERSION 3.0.2)
include_directories(../)
include_directories(../include/)
include_directories(${gtest_SOURCE_DIR}/include)

find_package(UUID REQUIRED)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS
  filesystem
  program_options
  regex
  system
)
include_directories(
  ${Boost_INCLUDE_DIRS}
  ${UUID_INCLUDE_DIRS}
)

set(SRC
  dag.cxx
  dag_algorithms.cxx
  dag_edge.cxx
  dag_vertex.cxx
  uuid.cxx
)

if (BUILD_SHARED_LIBS)
  add_library(u_dagtasks SHARED ${SRC})
  add_definitions(-DU_DAGTASKS_SHARED -DU_DAGTASKS_EXPORTS)
else()
  add_library(u_dagtasks STATIC ${SRC})
  add_definitions(-DU_DAGTASKS_STATIC)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/win32.cmake)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall \
    -fno-strict-aliasing")
endif()

target_link_libraries(u_dagtasks
  ${Boost_LIBRARIES}
  ${UUID_LIBRARIES}
)
