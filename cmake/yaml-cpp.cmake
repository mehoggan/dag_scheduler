# Download and unpack yaml-cpp at configure time
execute_process(COMMAND ${CMAKE_COMMAND}
  -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/yaml-cpp-download)
execute_process(COMMAND ${CMAKE_COMMAND} --build -YAML_BUILD_SHARED_LIBS=ON .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/yaml-cpp-download)

# Add yaml-cpp directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/yaml-cpp-src
  ${CMAKE_BINARY_DIR}/yaml-cpp-build)
