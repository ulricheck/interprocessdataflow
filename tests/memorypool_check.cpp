//
// Created by Ulrich Eck on 3/11/2015.
//
#include <iostream>
#include "memorypool_check.h"

// optionally extend text fixture
MemoryPoolTest::MemoryPoolTest() {}
MemoryPoolTest::~MemoryPoolTest() {};
void MemoryPoolTest::SetUp() {};
void MemoryPoolTest::TearDown() {};


TEST(MemoryPoolTest, MemoryPool_stresstest) {

    static const size_t n = 20000;
    static const size_t t = 8;
    static const size_t size = 32;

    std::cout << "buffer size: " << sizeof(ShmBufferRef<int>) << std::endl;

    blf::memorypool<ShmBufferRef<int>, blf::capacity<size> > pool;

    std::array<std::thread, t> threads;
    for (int i = 0; i < t; i++)
    {
        threads[i] = std::thread(&runner<ShmBufferRef<int>, size, n>, &pool);
    }

    for (int i = 0; i < t; i++)
    {
        threads[i].join();
        std::cout << "thread finished: " << i << std::endl;
    }

}