//
// Created by Ulrich Eck on 6/11/2015.
//

#ifndef IPDF_SHMBUFFERREF_H
#define IPDF_SHMBUFFERREF_H

#include "ipdf/ipdf.h"

namespace ipdf {

template<typename T>
struct ShmBufferRef {

    typedef T value_type;

    ShmBufferRef(ShmBufferRef const& v)
            :m_buffer_size(v.m_buffer_size), properties(v.properties), m_is_allocated(v.m_is_allocated), version(v.version),
             m_handle(v.m_handle) { }

    ShmBufferRef(void)
            :m_buffer_size(0), m_is_allocated(false), version(0) { }

    bool prepare(size_t s, bip::managed_shared_memory& msm)
    {
        if (!m_is_allocated) {
            try {
                auto data_ptr = msm.allocate(s);
                m_handle = msm.get_handle_from_address(data_ptr);
                m_buffer_size = s;
                m_is_allocated = true;
                return true;
            }
            catch (bip::bad_alloc& e) {
                BOOST_LOG_TRIVIAL(error) << "Error while allocating shared memory: " << e.what() << " code: " << e.get_error_code();
                return false;
            }
        }
        if (m_buffer_size!=s) {
            BOOST_LOG_TRIVIAL(error) << " invalid buffer size: " << s << " should be " << m_buffer_size;
            return false;
        }
        return true;
    }

    bool release(bip::managed_shared_memory& msm)
    {

        if (m_is_allocated) {
            msm.deallocate(msm.get_address_from_handle(m_handle));
            m_is_allocated = false;
//            m_handle = nullptr;
        }
        return true;
    }

    void* get_buffer(bip::managed_shared_memory& msm)
    {
        if (!m_is_allocated) {
            BOOST_LOG_TRIVIAL(warning) << "Buffer is not allocated!";
            return nullptr;
        }
        return msm.get_address_from_handle(m_handle);
    }

    size_t size() const
    {
        return m_buffer_size;
    }

    bool is_allocated()
    {
        return m_is_allocated;
    }

    unsigned int version;
    T properties;

private:
    size_t m_buffer_size;
    bool m_is_allocated;
    bip::managed_shared_memory::handle_t m_handle;
};

} // end namespace

#endif //IPDF_SHMBUFFERREF_H
