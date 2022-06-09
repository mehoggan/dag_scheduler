set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-conversion")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  link_directories("${CMAKE_BINARY_DIR}/openssl/lib")
  include_directories("${CMAKE_BINARY_DIR}/openssl/include")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-declarations")
endif()

include_directories("${CMAKE_BINARY_DIR}/yaml-cpp-build/include"
  "${CMAKE_BINARY_DIR}/yaml-cpp-src/include"
  "${CMAKE_CURRENT_SOURCE_DIR}/../include"
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
)

add_executable(main "./main.cpp")

target_link_libraries(main
    dagtasks
    yaml-cpp
    boost_system
    boost_thread
    boost_chrono
    ssl.3
    crypto
)

configure_file("${CMAKE_CURRENT_LIST_DIR}/resources/server.yml"
    "${CMAKE_CURRENT_BINARY_DIR}/resources/server.yml"
    COPYONLY
)
