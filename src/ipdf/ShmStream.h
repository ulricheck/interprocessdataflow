//
// Created by Ulrich Eck on 7/11/2015.
//

#ifndef IPDF_SHMSTREAM_H
#define IPDF_SHMSTREAM_H

#include "ipdf/ipdf.h"

template<typename T, size_t N>
class ShmStream {

    typedef bip::allocator<T, bip::managed_shared_memory::segment_manager> queue_allocator;
    typedef blf::spsc_queue<T, blf::capacity<N> > message_queue_t;

public:



private:
    message_queue_t m_queue;
};
#endif //IPDF_SHMSTREAM_H
