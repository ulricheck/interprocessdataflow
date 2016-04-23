cmake_minimum_required(VERSION 3.3)
SET(PROJECT_NAME "ipdf")
project(${PROJECT_NAME})

include(conanbuildinfo.cmake)
conan_basic_setup()

IF(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF(UNIX)

include_directories(src)

enable_testing()
add_subdirectory(tests)

