set (SYSTEM_LIBS "")
if(UNIX)
  SET (SYSTEM_LIBS ${SYSTEM_LIBS} "rt")
endif()

# two simple examples for now
set(SOURCE_FILES_TEST_PRODUCER1 producer1.cpp)
add_executable(ipdf_test_producer1 ${SOURCE_FILES_TEST_PRODUCER1})
target_link_libraries(ipdf_test_producer1 CONAN_PKG::Boost CONAN_PKG::catch2 ${SYSTEM_LIBS})
set_property(TARGET ipdf_test_producer1 PROPERTY CXX_STANDARD 11)

set(SOURCE_FILES_TEST_PRODUCER2 producer2.cpp)
add_executable(ipdf_test_producer2 ${SOURCE_FILES_TEST_PRODUCER2})
target_link_libraries(ipdf_test_producer2  CONAN_PKG::Boost CONAN_PKG::catch2 ${SYSTEM_LIBS})
set_property(TARGET ipdf_test_producer2 PROPERTY CXX_STANDARD 11)

set(SOURCE_FILES_TEST_CONSUMER consumer.cpp)
add_executable(ipdf_test_consumer ${SOURCE_FILES_TEST_CONSUMER})
target_link_libraries(ipdf_test_consumer  CONAN_PKG::Boost CONAN_PKG::catch2 ${SYSTEM_LIBS})
set_property(TARGET ipdf_test_consumer PROPERTY CXX_STANDARD 11)

add_executable(ifpdf_tests
        main.cpp
        test_port.cpp
        test_bufferpool.cpp
        test_stream.cpp
        )

target_link_libraries(ifpdf_tests  CONAN_PKG::Boost CONAN_PKG::catch2 ${SYSTEM_LIBS})
set_property(TARGET ifpdf_tests PROPERTY CXX_STANDARD 11)

IF(WIN32) # Check if we are on Windows
  if(MSVC) # Check if we are using the Visual Studio compiler
    set_target_properties(ifpdf_tests PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:CONSOLE")
  elseif(CMAKE_COMPILER_IS_GNUCXX)
    # SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwindows") # Not tested
  else()
    message(SEND_ERROR "You are using an unsupported Windows compiler! (Not MSVC or GCC)")
  endif()
elseif(UNIX)
  # Nothing special required
else()
  message(SEND_ERROR "You are on an unsupported platform! (Not Win32 or Unix)")
ENDIF()

add_test(NAME ifpdf_tests COMMAND ifpdf_tests)


