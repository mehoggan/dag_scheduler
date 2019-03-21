cmake_minimum_required(VERSION 3.0.2)
include_directories(../)
include_directories(../include/)
message(STATUS "gtest_SOURCE_DIR = ${gtest_SOURCE_DIR}")
include_directories(${gtest_SOURCE_DIR}/include)

find_package(UUID REQUIRED)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)

set(SRC
  ./concurrent_task_queue.cxx
  ./dag.cxx
  ./dag_algorithms.cxx
  ./dag_edge.cxx
  ./dag_vertex.cxx
  ./interruptible_task_thread.cxx
  ./logging.cxx
  ./stop_watch.cxx
  ./task.cxx
  ./task_scheduler.cxx
  ./task_stage.cxx
  ./uuid.cxx
)

if (BUILD_SHARED_LIBS)
  add_library(dagtasks SHARED ${SRC})
  add_definitions(-DDAGTASKS_SHARED -DDAGTASKS_EXPORTS)
else()
  add_library(dagtasks STATIC ${SRC})
  add_definitions(-DDAGTASKS_STATIC)
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/win32.cmake)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall \
    -fno-strict-aliasing")
endif()

# We explicitly list boost libraries because of boost beast.
target_link_libraries(dagtasks
  ${UUID_LIBRARIES}
  boost_date_time
  boost_filesystem
  boost_log
  boost_log_setup
  boost_system
  boost_thread
  boost_program_options
  pthread
)
add_dependencies(dagtasks gtest)
