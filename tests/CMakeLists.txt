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
