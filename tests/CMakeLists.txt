cmake_minimum_required(VERSION 3.0.2)

find_program(valgrind REQUIRED)
set(CTEST_MEMORYCHECK_COMMAND "/usr/bin/valgrind") 
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes
  --leak-check=full")
include(CTest)
enable_testing()

add_test(unit_test unit_test --report_level=no --log_level=all
  --output_format=xml --log_sink=${CMAKE_BINARY_DIR}/test_logs/${name}.xml)

set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package(Boost REQUIRED COMPONENTS
  date_time
  filesystem
  log
  log_setup
  system
  thread
  program_options
)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(../include)
include_directories(../)
include_directories(${gtest_SOURCE_DIR}/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/win32.cmake)
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror -Wall \
    -fno-strict-aliasing")
endif()

add_executable(unit_test
  main.cxx
  test_concurrent_queue.cxx
  test_dag.cxx
  test_dag_algorithms.cxx
  test_dag_edge.cxx
  test_dag_vertex.cxx
  test_logging.cxx
  test_uuid.cxx
)

target_link_libraries(unit_test
  dagtasks
  gtest
  gtest_main
  gmock
  gmock_main
  ${Boost_LIBRARIES}
)
