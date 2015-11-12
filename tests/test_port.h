//
// Created by Ulrich Eck on 10/11/2015.
//

#ifndef IPDF_TEST_PORT_H
#define IPDF_TEST_PORT_H

#include "gtest/gtest.h"
#include "ipdf/ipdf.h"
#include "ipdf/ShmPort.h"

#include <iostream>
#include <thread>
#include <vector>
#include <random>


using namespace ipdf;

template <typename T, size_t size, size_t n, size_t bs>
void port_runner_producer (ShmPort* stream)
{
    BOOST_LOG_TRIVIAL(info) << "nothing there yet ...";
};

// The fixture for testing class Foo.
class PortTest: public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    PortTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~PortTest();

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
#endif //IPDF_TEST_PORT_H
