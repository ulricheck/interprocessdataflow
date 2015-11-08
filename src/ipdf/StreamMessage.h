//
// Created by Ulrich Eck on 8/11/2015.
//

#ifndef IPDF_STREAMMESSAGE_H
#define IPDF_STREAMMESSAGE_H

#include "ipdf/ipdf.h"

namespace ipdf {


template<typename T>
struct StreamMessage {
  typedef std::allocator<T> allocator_type;

  StreamMessage()
          :timestamp(0), size(sizeof(T)) { }

  StreamMessage(const unsigned long long& ts, T const& msg)
          :timestamp(ts), size(sizeof(T)), message(msg) { }

  StreamMessage(const StreamMessage & other)
          : timestamp(other.timestamp), size(other.size), message(other.message) { }

  unsigned long long timestamp;
  size_t size;
  T message;
};


}

#endif //IPDF_STREAMMESSAGE_H
