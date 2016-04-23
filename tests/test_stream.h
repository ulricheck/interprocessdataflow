//
// Created by Ulrich Eck on 8/11/2015.
//

#ifndef IPDF_TEST_STREAM_H
#define IPDF_TEST_STREAM_H

#include "ipdf/ipdf.h"
#include "ipdf/StreamMessage.h"
#include "ipdf/ShmStream.h"

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

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

#endif //IPDF_TEST_STREAM_H
