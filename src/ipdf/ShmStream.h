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

    ShmStream(const char* name, const std::shared_ptr<bip::managed_shared_memory>& msm) :
            m_queue_name(name),
            m_queue(nullptr),
            m_memory_manager(msm)
    {}

    bool allocate() {
        try {
            m_queue = m_memory_manager->find_or_construct<message_queue_t>(m_queue_name)();
            return true;
        } catch (bip::bad_alloc &e) {
            BOOST_LOG_TRIVIAL(error) << "error while allocating ShmStream: " << m_queue_name << " - " << e.what();
        }
        return false;
    }

    bool deallocate() {
        if (m_queue) {
            m_memory_manager->destroy_ptr(m_queue);
            m_queue = nullptr;
        }
        return true;
    }

    bool send(const T & value) {
        if (m_queue) {
            return m_queue->push(value);
        }
        return false;
    }

    unsigned long receive(T & value) {
        if (m_queue) {
            return m_queue->pop(value);
        }
        return false;
    }

    template <typename Functor>
    unsigned long receive_many(Functor & f)
    {
        return m_queue->consume_all(f);
    }

private:
    const char* m_queue_name;
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;

    message_queue_t* m_queue;
};
#endif //IPDF_SHMSTREAM_H
