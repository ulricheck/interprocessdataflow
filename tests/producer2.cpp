//
// Created by Ulrich Eck on 28/10/2015.
//


#include "ipdf/ipdf.h"
#include "ipdf/ShmPortCondition.h"

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/thread/thread.hpp>

int main()
{
    // create segment and corresponding allocator
    bip::managed_shared_memory segment(bip::open_or_create, "ipdf_testdomain_simpletest", 65536);
    ipdf::ShmPortCondition *port = segment.find_or_construct<ipdf::ShmPortCondition>("port_testing")();

    shm::shmem_channel_allocator alloc(segment.get_segment_manager());
    shm::channel_message_queue *queue = segment.find_or_construct<shm::channel_message_queue>("queue2")();



    for (unsigned int i = 0; i < 1000; i++)
    {
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));

        std::vector<int64_t> buffers(3);
        buffers.at(0) = (int64_t)i;
        buffers.at(1) = (int64_t)(i + 1);
        buffers.at(2) = (int64_t)(i + 2);

        queue->push(shm::ChannelMessage((unsigned long long)i*100, buffers, alloc));

        // should be atomic swap
        {
            bip::scoped_lock<bip::interprocess_mutex> lock(port->port_mutex);
            port->data_available = true;
        }
        port->port_condition.notify_all();
    }
}