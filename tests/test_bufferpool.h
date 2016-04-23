//
// Created by Ulrich Eck on 3/11/2015.
//
#ifndef IPDF_MEMORYPOOL_CHECK_H_H
#define IPDF_MEMORYPOOL_CHECK_H_H

#include "ipdf/ipdf.h"
#include "ipdf/ShmBufferPool.h"
#include "ipdf/ShmBufferRef.h"

#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <iostream>
#include <thread>
#include <vector>
#include <random>


using namespace ipdf;

template <typename T, size_t size, size_t n, size_t bs>
void bufferpool_runner (ShmBufferPool<T, size >* pool)
{
    std::vector<T> vec;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<typename T::value_type> dist(0, 6);

    uint64_t buffer_hits = 0;
    uint64_t buffer_underrruns = 0;
    uint64_t buffer_returns = 0;


    for (size_t i = 0; i < n; i++)
    {
        T buf;
        switch (dist(generator))
        {
        case 0:
        case 1:
        case 2:
        case 3:
            if (pool->obtain(buf, bs)) {
                vec.push_back(buf);
                buffer_hits++;
            } else {
                buffer_underrruns++;
            }
            break;
        case 4:
            if (!vec.empty())
                vec[generator() % vec.size()].properties = rand();
            break;
        case 5:
            if (!vec.empty())
            {
                BOOST_TEST(pool->release(vec.back()));
                vec.pop_back();
                buffer_returns++;
            }
            break;
        case 6:
            if (!vec.empty())
            {
                for(auto p : vec) {
                    BOOST_TEST(pool->release(p));
                    buffer_returns++;
                }
                vec.clear();
            }
            break;
        }
    }
    // check buffer_hits
    // check buffer_underruns
    // check buffer_returns
    BOOST_TEST( true ); // dummy for now
    BOOST_LOG_TRIVIAL(info) << "buffer hits: " << buffer_hits << " underruns: " << buffer_underrruns <<  " returns: " << buffer_returns;
};


#endif //IPDF_MEMORYPOOL_CHECK_H_H
