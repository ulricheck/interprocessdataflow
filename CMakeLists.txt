cmake_minimum_required(VERSION 3.3)
SET(PROJECT_NAME "ipdf")
project(${PROJECT_NAME})

# maybe use it in the future ..
#include(conanbuildinfo.cmake)
#conan_basic_setup()

IF(UNIX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
ENDIF(UNIX)

set(requiredLibs "")

set(Boost_USE_STATIC_LIBS        ON) # only find static libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)

find_package(Boost COMPONENTS system date_time log thread REQUIRED)
if(Boost_FOUND)
   add_definitions("-DBOOST_ALL_NO_LIB")
   include_directories(${Boost_INCLUDE_DIRS})
   set(requiredLibs ${requiredLibs} ${Boost_LIBRARIES})
endif()

include_directories(src)

enable_testing()
add_subdirectory(tests)

