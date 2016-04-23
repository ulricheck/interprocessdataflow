//
// Created by Ulrich Eck on 3/11/2015.
//

#include <boost/test/unit_test.hpp>
#include "ipdf_test.h"

using boost::unit_test::test_suite;

// this function replaces the C++ main function, which is implemented by BOOST
// run with --log_level=all to get some more output
test_suite* init_unit_test_suite( int, char* [] )
{
	// create a test suite
	test_suite* allTests = BOOST_TEST_SUITE( "ipdf_all_tests" );

	// this example will pass cause we know ahead of time number of expected failures
	allTests->add( new IPDFTest );
	
	return allTests;
}