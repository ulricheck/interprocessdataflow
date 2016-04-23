
# two simple examples for now
set(SOURCE_FILES_TEST_PRODUCER1 producer1.cpp)
add_executable(ipdf_test_producer1 ${SOURCE_FILES_TEST_PRODUCER1})
target_link_libraries(ipdf_test_producer1 ${CONAN_LIBS})

set(SOURCE_FILES_TEST_PRODUCER2 producer2.cpp)
add_executable(ipdf_test_producer2 ${SOURCE_FILES_TEST_PRODUCER2})
target_link_libraries(ipdf_test_producer2 ${CONAN_LIBS})

set(SOURCE_FILES_TEST_CONSUMER consumer.cpp)
add_executable(ipdf_test_consumer ${SOURCE_FILES_TEST_CONSUMER})
target_link_libraries(ipdf_test_consumer ${CONAN_LIBS})

add_definitions(-DBOOST_TEST_DYN_LINK)
add_executable(ifpdf_tests
        main.cpp
#        boost_unit_test_framework.cpp
        ipdf_test.cpp
        test_port.cpp
        test_bufferpool.cpp
        test_stream.cpp
        )

target_link_libraries(ifpdf_tests ${CONAN_LIBS} )

set_property(TARGET ifpdf_tests PROPERTY CXX_STANDARD 11)

add_test(NAME ifpdf_tests COMMAND ifpdf_tests)


