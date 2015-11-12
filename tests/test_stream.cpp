//
// Created by Ulrich Eck on 8/11/2015.
//

#include <iostream>
#include "test_stream.h"

// optionally extend text fixture
StreamTest::StreamTest() {}
StreamTest::~StreamTest() {};
void StreamTest::SetUp() {};
void StreamTest::TearDown() {};


TEST(StreamTest, Stream_stresstest) {

    static const size_t n = 20000;
    static const size_t t = 8;
    static const size_t size = 32;
    static const size_t buffer_size = 8096;

    typedef ShmStream< StreamMessage<int>, size > stream_t;

    struct shm_remove
    {
      shm_remove() { bip::shared_memory_object::remove("SHM_TEST_IPDF_STREAM"); }
      ~shm_remove(){ bip::shared_memory_object::remove("SHM_TEST_IPDF_STREAM"); }
    } remover;


    const char *SHM_MEMORY_NAME = "SHM_TEST_IPDF_STREAM";
    const char *SHM_CHANNEL_NAME = "TESTCHANNEL";
    const char *SHM_STREAM_NAME = "TESTSTREAM";

    size_t shm_size = sizeof(stream_t) + 8096 * 10;

    std::shared_ptr< bip::managed_shared_memory > msm(new bip::managed_shared_memory(bip::open_or_create, SHM_MEMORY_NAME, shm_size));
    std::unique_ptr< stream_t > stream(new stream_t(SHM_CHANNEL_NAME, SHM_STREAM_NAME, msm));
    stream->allocate();

    //std::thread(&runner<ShmBufferRef<int>, size, n, buffer_size>, pool.get());

    // delete pool
    stream->deallocate();
    stream.reset(nullptr);

    BOOST_LOG_TRIVIAL(info) << "MSM all dealocated: " << msm->all_memory_deallocated();
}