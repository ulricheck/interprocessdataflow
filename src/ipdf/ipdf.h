//
// Created by Ulrich Eck on 28/10/2015.
//

#ifndef IPDF_IPDF_H
#define IPDF_IPDF_H

#include <boost/lockfree/spsc_queue.hpp> // ring buffer

#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/string.hpp>
#include <boost/interprocess/containers/vector.hpp>

#include <boost/range.hpp>

namespace bip = boost::interprocess;
namespace shm {
typedef bip::allocator<char, bip::managed_shared_memory::segment_manager> char_alloc;
typedef bip::basic_string<char, std::char_traits<char>, char_alloc> shared_string;

typedef boost::lockfree::spsc_queue <
shared_string,
boost::lockfree::capacity<200>
> ring_buffer;


// implement channel queue

typedef bip::allocator<int64_t, bip::managed_shared_memory::segment_manager> int_alloc;
typedef bip::vector<int64_t, int_alloc> shmem_int_vector;

struct ChannelMessage {
  typedef int_alloc allocator_type;

  ChannelMessage(int_alloc alloc) : timestamp(0), active_buffers(alloc) {}

  template<typename ListType>
  ChannelMessage(const unsigned long long& ts, ListType const& ab, int_alloc alloc)
          : timestamp(ts), active_buffers(boost::begin(ab), boost::end(ab), alloc)
  { }

  unsigned long long timestamp;
  shmem_int_vector active_buffers;
};

typedef boost::interprocess::allocator<ChannelMessage, bip::managed_shared_memory::segment_manager> shmem_channel_allocator;
typedef boost::lockfree::spsc_queue<ChannelMessage, boost::lockfree::capacity<128> > channel_message_queue;
}

#endif //IPDF_IPDF_H
