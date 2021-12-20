# Download and unpack aws-sdk-cpp at configure time
execute_process(COMMAND ${CMAKE_COMMAND}
  -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/aws-sdk-cpp-download)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/aws-sdk-cpp-download)

# Add aws-sdk-cpp directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/aws-sdk-cpp-src
  ${CMAKE_BINARY_DIR}/aws-sdk-cpp-build)
