# Download and unpack cpprestsdk at configure time
execute_process(COMMAND ${CMAKE_COMMAND}
  -G "${CMAKE_GENERATOR}" .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cpprestsdk-download)
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/cpprestsdk-download)

# Add cpprestsdk directly to our build. This adds
# the following targets: gtest, gtest_main, gmock
# and gmock_main
add_subdirectory(${CMAKE_BINARY_DIR}/cpprestsdk-src
  ${CMAKE_BINARY_DIR}/cpprestsdk-build)