cmake_minimum_required(VERSION 3.3)
SET(PROJECT_NAME "ipdf")
project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
SET(CMAKE_MODULE_PATH  ${${PROJECT_NAME}_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

SET(requiredLibs)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME OFF)
find_package( Boost 1.58 COMPONENTS thread system chrono date_time program_options log REQUIRED)
if(Boost_FOUND)
    add_definitions("-DBOOST_ALL_NO_LIB")
    add_definitions(-DHAVE_BOOST)
    SET(HAVE_BOOST 1)
    link_directories(${Boost_LIBRARY_DIRS})
    message (STATUS "  Boost_LIBRARIES: ${Boost_LIBRARIES}")
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIR}")
endif(Boost_FOUND)
set(requiredLibs ${requiredLibs} ${Boost_LIBRARIES})
include_directories(${Boost_INCLUDE_DIR})


include_directories(src)

enable_testing()
add_subdirectory(tests)

