//
// Created by Ulrich Eck on 10/11/2015.
//


#include <iostream>
#include "test_port.h"
#include "ipdf/StreamMessage.h"

// optionally extend text fixture
PortTest::PortTest() {}
PortTest::~PortTest() {};
void PortTest::SetUp() {};
void PortTest::TearDown() {};


TEST(PortTest, Port_assemblytest) {

    static const size_t port_ringbuffer_size = 4096;
    static const size_t buffer_size = 8096;
    static const

    struct shm_remove
    {
      shm_remove() { bip::shared_memory_object::remove("SHM_TEST_IPDF_PORT"); }
      ~shm_remove(){ bip::shared_memory_object::remove("SHM_TEST_IPDF_PORT"); }
    } remover;

    const char *SHM_MEMORY_NAME = "SHM_TEST_IPDF_PORT";
    const char *SHM_PORT_NAME = "TEST_PORT";
    const char *SHM_CH0_NAME = "CH0";
    const char *SHM_CH1_NAME = "CH1";
    const char *SHM_ST0_NAME = "ST0";
    const char *SHM_ST1_NAME = "ST1";

    typedef ipdf::StreamMessage<int> msg1_t;
    typedef ipdf::StreamMessage<double> msg2_t;

    size_t shm_size = buffer_size * 1000;

    std::shared_ptr< bip::managed_shared_memory > msm(new bip::managed_shared_memory(bip::open_or_create, SHM_MEMORY_NAME, shm_size));
    std::unique_ptr< ipdf::ShmPort > port(new ipdf::ShmPort(SHM_PORT_NAME, msm));

    int ch0_idx = port->addChannel(SHM_CH0_NAME);
    std::shared_ptr<ipdf::ShmStream<msg1_t, 128> > ch0st0 = port->getChannel(ch0_idx)->addStream<msg1_t, 128>(SHM_ST0_NAME);
    std::shared_ptr<ipdf::ShmStream<msg2_t, 64> > ch0st1 = port->getChannel(ch0_idx)->addStream<msg2_t, 64>(SHM_ST1_NAME);

    int ch1_idx = port->addChannel(SHM_CH1_NAME);
    std::shared_ptr<ipdf::ShmStream<msg1_t, 128> > ch1st0 = port->getChannel(ch1_idx)->addStream<msg1_t, 128>(SHM_ST0_NAME);
    std::shared_ptr<ipdf::ShmStream<msg2_t, 64> > ch1st1 = port->getChannel(ch1_idx)->addStream<msg2_t, 64>(SHM_ST1_NAME);

    port->allocate();

    //std::thread(&runner<ShmBufferRef<int>, size, n, buffer_size>, pool.get());

    // delete pool
    port->deallocate();
    port.reset(nullptr);

    BOOST_LOG_TRIVIAL(info) << "MSM all dealocated: " << msm->all_memory_deallocated();
}