#include "ipdf_test.h"

// declare external tests here, to save us some trivial header files
void TestBufferPool();
void TestPort();
void TestStream();


IPDFTest::IPDFTest()
	: boost::unit_test::test_suite( "IPDFTests" )
{
	add( BOOST_TEST_CASE( &TestBufferPool ) );
	add( BOOST_TEST_CASE( &TestPort ) );
	add( BOOST_TEST_CASE( &TestStream ) );
}
