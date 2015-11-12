//
// Created by Ulrich Eck on 8/11/2015.
//

#ifndef IPDF_TEST_STREAM_H
#define IPDF_TEST_STREAM_H

#include "gtest/gtest.h"
#include "ipdf/ipdf.h"
#include "ipdf/StreamMessage.h"
#include "ipdf/ShmStream.h"

#include <iostream>
#include <thread>
#include <vector>
#include <random>


using namespace ipdf;

template <typename T, size_t size, size_t n, size_t bs>
void stream_runner_producer (ShmStream<T, size >* stream)
{
    uint64_t msgs_received = 0;


    for (size_t i = 0; i < n; i++)
    {

    }
    BOOST_LOG_TRIVIAL(info) << "to be done ..";
};

// The fixture for testing class Foo.
class StreamTest: public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    StreamTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~StreamTest();

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    virtual void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    virtual void TearDown();

    // The mock bar library shaed by all tests
    //MockBar m_bar;
};

#endif //IPDF_TEST_STREAM_H
