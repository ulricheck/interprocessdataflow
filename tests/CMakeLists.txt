# get dependencies
find_package(Threads REQUIRED)

# Enable ExternalProject CMake module
include(ExternalProject)

# Download and install GoogleTest
ExternalProject_Add(
        gtest
        URL https://googletest.googlecode.com/files/gtest-1.7.0.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
        # Disable install step
        INSTALL_COMMAND ""
)

# Create a libgtest target to be used as a dependency by test programs
add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest)

# Set gtest properties
ExternalProject_Get_Property(gtest source_dir binary_dir)
set_target_properties(libgtest PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/libgtest.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        #    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
        )
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

# Download and install GoogleMock
ExternalProject_Add(
        gmock
        URL https://googlemock.googlecode.com/files/gmock-1.7.0.zip
        PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gmock
        # Disable install step
        INSTALL_COMMAND ""
)

# Create a libgmock target to be used as a dependency by test programs
add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock gmock)

# Set gmock properties
ExternalProject_Get_Property(gmock source_dir binary_dir)
set_target_properties(libgmock PROPERTIES
        "IMPORTED_LOCATION" "${binary_dir}/libgmock.a"
        "IMPORTED_LINK_INTERFACE_LIBRARIES" "${CMAKE_THREAD_LIBS_INIT}"
        #    "INTERFACE_INCLUDE_DIRECTORIES" "${source_dir}/include"
        )
# I couldn't make it work with INTERFACE_INCLUDE_DIRECTORIES
include_directories("${source_dir}/include")

# two simple examples for now
set(SOURCE_FILES_TEST_PRODUCER1 producer1.cpp)
add_executable(ipdf_test_producer1 ${SOURCE_FILES_TEST_PRODUCER1})
target_link_libraries(ipdf_test_producer1 ${requiredLibs})

set(SOURCE_FILES_TEST_PRODUCER2 producer2.cpp)
add_executable(ipdf_test_producer2 ${SOURCE_FILES_TEST_PRODUCER2})
target_link_libraries(ipdf_test_producer2 ${requiredLibs})

set(SOURCE_FILES_TEST_CONSUMER consumer.cpp)
add_executable(ipdf_test_consumer ${SOURCE_FILES_TEST_CONSUMER})
target_link_libraries(ipdf_test_consumer ${requiredLibs})


add_executable(ifpdf_tests
        test_main.cpp
        test_bufferpool.cpp
        test_stream.cpp
        )

target_link_libraries(
        ifpdf_tests
        libgtest
        libgmock
        )

target_link_libraries(ifpdf_tests ${requiredLibs})

add_test(NAME ifpdf_tests
        COMMAND ifpdf_tests)