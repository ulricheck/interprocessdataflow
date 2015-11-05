//
// Created by Ulrich Eck on 3/11/2015.
//
#ifndef IPDF_MEMORYPOOL_CHECK_H_H
#define IPDF_MEMORYPOOL_CHECK_H_H

#include "gtest/gtest.h"
#include "ipdf/ipdf.h"
#include "ipdf/memorypool.h"
#include "ipdf/ShmBufferRef.h"

#include <iostream>
#include <thread>
#include <vector>
#include <random>




template <typename T, size_t size, size_t n>
void runner (blf::memorypool<T, blf::capacity<size> > *pool)
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
            if (pool->obtain(buf)) {
                if (buf.prepare(1024)) {
                    vec.push_back(buf);
                    buffer_hits++;
                }
            } else {
                buffer_underrruns++;
            }
            break;
        case 4:
            if (!vec.empty())
                vec[generator() % vec.size()].data = rand();
            break;
        case 5:
            if (!vec.empty())
            {
                assert(pool->release(vec.back()));
                vec.pop_back();
                buffer_returns++;
            }
            break;
        case 6:
            if (!vec.empty())
            {
                for(auto p : vec) {
                    assert(pool->release(p));
                    buffer_returns++;
                }
                vec.clear();
            }
            break;
        }
    }
    std::cout << "buffer hits: " << buffer_hits << " underruns: " << buffer_underrruns <<  " returns: " << buffer_returns << std::endl;
};

// The fixture for testing class Foo.
class MemoryPoolTest: public ::testing::Test {

protected:

    // You can do set-up work for each test here.
    MemoryPoolTest();

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~MemoryPoolTest();

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

#endif //IPDF_MEMORYPOOL_CHECK_H_H
