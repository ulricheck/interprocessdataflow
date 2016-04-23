//
// Created by Ulrich Eck on 10/11/2015.
//

#ifndef IPDF_TEST_PORT_H
#define IPDF_TEST_PORT_H


#include "ipdf/ipdf.h"
#include "ipdf/ShmPort.h"

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

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

#endif //IPDF_TEST_PORT_H
