//
// Created by Ulrich Eck on 6/11/2015.
//

#ifndef IPDF_SHMBUFFERREF_H
#define IPDF_SHMBUFFERREF_H

#include "ipdf/ipdf.h"

template< typename T >
struct ShmBufferRef {

  typedef T value_type;

  ShmBufferRef(ShmBufferRef const & v)
          : buffer_size(v.buffer_size)
          , data(v.data)
          , is_allocated(v.is_allocated)
          , version(v.version)
          , handle(v.handle)
  {}

  ShmBufferRef(void)
          : buffer_size(0)
          , is_allocated(false)
          , version(0)
  {}

  bool prepare(size_t s) {
      if (!is_allocated) {
          std::cout << " allocate referenced buffer: " << s << " bytes." << std::endl;
          buffer_size = s;
          is_allocated = true;
          return true;
      }
      if (buffer_size != s) {
          std::cout << " invalid buffer size: " << s << " should be " << buffer_size << std::endl;
          return false;
      }
      return true;
  }

  size_t buffer_size;
  bool is_allocated;
  unsigned int version;
  T data;
  bip::managed_shared_memory::handle_t handle;
};


#endif //IPDF_SHMBUFFERREF_H
