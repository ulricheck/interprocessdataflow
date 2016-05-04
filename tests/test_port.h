//
// Created by Ulrich Eck on 10/11/2015.
//

#ifndef IPDF_TEST_PORT_H
#define IPDF_TEST_PORT_H

#include "ipdf/catch.hpp"

#include "ipdf/ipdf.h"
#include "ipdf/ShmPort.h"

#include <iostream>
#include <vector>
#include <random>


using namespace ipdf;

template <typename T, size_t size, size_t n, size_t bs>
void port_runner_producer (ShmPort* stream)
{
    BOOST_LOG_TRIVIAL(info) << "nothing there yet ...";
};

#endif //IPDF_TEST_PORT_H
