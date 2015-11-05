//
// Created by Ulrich Eck on 28/10/2015.
//

#include "ipdf/ipdf.h"
#include "ipdf/ShmPort.h"

#include "boost/date_time/posix_time/posix_time.hpp"
#include <boost/thread/thread_time.hpp>

#include <iostream>

int main()
{

    //Remove shared memory on construction and destruction
    struct shm_remove
    {
      shm_remove() { bip::shared_memory_object::remove("ipdf_testdomain_simpletest"); }
      ~shm_remove(){ bip::shared_memory_object::remove("ipdf_testdomain_simpletest"); }
    } remover;

    // create segment and corresponding allocator
    bip::managed_shared_memory segment(bip::open_or_create, "ipdf_testdomain_simpletest", 65536);
    ShmPort *port = segment.find_or_construct<ShmPort>("port_testing")();

    shm::shmem_channel_allocator alloc(segment.get_segment_manager());
    shm::channel_message_queue *queue1 = segment.find_or_construct<shm::channel_message_queue>("queue1")();
    shm::channel_message_queue *queue2 = segment.find_or_construct<shm::channel_message_queue>("queue2")();


    bool running = true;

    unsigned int total_message_count = 0;

    while (running)
    {
        bool process_queues = false;
        // wait for activity .. could also be polling ...
        {
            bip::scoped_lock<bip::interprocess_mutex> lock(port->port_mutex);
            if (!port->data_available) {
                boost::posix_time::ptime timeout = boost::get_system_time() + boost::posix_time::milliseconds(1);
                bool was_notified = port->port_condition.timed_wait(lock, timeout);
                if (was_notified) {
                    process_queues = true;
                }
                port->data_available = false;
            }
        }

        std::vector<unsigned long long> messages(0);

        if (process_queues) {
            std::cout << "process_queues" << "\n";
            bool continue_poll = true;
            shm::ChannelMessage v(alloc);
            // loop until no more values are available
            while (continue_poll) {
                continue_poll = false;
                if (queue1->pop(v)) {
                    //std::cout << "Processed 1: '" << v.timestamp << "'\n";
                    messages.push_back(v.timestamp);
                    continue_poll = true;
                }
                if (queue2->pop(v)) {
//                    std::cout << "Processed 2: '" << v.timestamp << "'\n";
                    messages.push_back(v.timestamp);
                    continue_poll = true;
                }
            }
        }

        if (messages.size() > 0) {
            total_message_count += messages.size();
            std::cout << " received " << total_message_count << " messages in total \n";
        }
    }
}