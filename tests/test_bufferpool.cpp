//
// Created by Ulrich Eck on 3/11/2015.
//

#include <iostream>
#include "test_bufferpool.h"


TEST_CASE( "Testing BufferPool", "[ipdf_basic]" ) {

    static const size_t n = 20000;
    static const size_t t = 8;
    static const size_t size = 32;
    static const size_t buffer_size = 8096;

    typedef ShmBufferPool< ShmBufferRef<int>, size > pool_t;

    struct shm_remove
    {
      shm_remove() { bip::shared_memory_object::remove("SHM_TEST_IPDF_MEMORYPOOL"); }
      ~shm_remove(){ bip::shared_memory_object::remove("SHM_TEST_IPDF_MEMORYPOOL"); }
    } remover;


    const char *SHM_MEMORY_NAME = "SHM_TEST_IPDF_MEMORYPOOL";
    const char *SHM_POOL_NAME = "TEST_POOL";

    size_t shm_size = size * (sizeof(ShmBufferRef<int>) + buffer_size) + 8096 * 10;

    std::shared_ptr< bip::managed_shared_memory > msm(new bip::managed_shared_memory(bip::open_or_create, SHM_MEMORY_NAME, shm_size));
    std::unique_ptr< pool_t > pool(new pool_t(SHM_POOL_NAME, buffer_size, msm));
    pool->allocate();

    std::array<std::thread, t> threads;

    for (int i = 0; i < t; i++)
    {
        threads[i] = std::thread(&bufferpool_runner<ShmBufferRef<int>, size, n, buffer_size>, pool.get());
    }

    for (int i = 0; i < t; i++)
    {
        threads[i].join();
        BOOST_LOG_TRIVIAL(info) << "thread finished: " << i;
    }

    // delete pool
    pool->deallocate();
    pool.reset(nullptr);

    BOOST_LOG_TRIVIAL(info) << "MSM all dealocated: " << msm->all_memory_deallocated();
    REQUIRE(msm->all_memory_deallocated() == true);
}