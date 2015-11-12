//
// Created by Ulrich Eck on 7/11/2015.
//

#ifndef IPDF_SHMSTREAM_H
#define IPDF_SHMSTREAM_H

#include "ipdf/ipdf.h"
#include "ipdf/ShmBufferPool.h"
#include "ipdf/ShmBufferRef.h"
#include <functional>

namespace ipdf {

class ShmStreamBase : boost::noncopyable {

public:

    ShmStreamBase(const char* channel_name, const char* stream_name, const std::shared_ptr<bip::managed_shared_memory>& msm) :
            m_channel_name(channel_name),
            m_stream_name(stream_name),
            m_memory_manager(msm) { }

    virtual bool allocate() { return false; }
    virtual bool deallocate() { return false; }

    virtual void recycle_buffers() { }

    // more public interfaces .. e.g. cast to shared_ptr<void*> ?

    virtual std::string getName() {
        std::string s_name = m_channel_name + std::string("_") + m_stream_name;
        return s_name;
    }


protected:

    template<typename T>
    T* allocate_shm(const char* name) {
        return m_memory_manager->find_or_construct<T>(name)();
    }

    template<typename T>
    void deallocate_shm(T* ptr) {
        m_memory_manager->destroy_ptr(ptr);
    }

    const std::string m_channel_name;
    const std::string m_stream_name;
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
};




template<typename T, size_t N>
class ShmStream : public ShmStreamBase {

    typedef bip::allocator<T, bip::managed_shared_memory::segment_manager> queue_allocator;
    typedef blf::spsc_queue<T, blf::capacity<N> > message_queue_t;

public:

    ShmStream(const char* channel_name, const char* stream_name, const std::shared_ptr<bip::managed_shared_memory>& msm) :
            ShmStreamBase(channel_name, stream_name, msm),
            m_queue(nullptr)
    { }

    virtual bool allocate() {
        BOOST_LOG_TRIVIAL(debug) << "Allocate Stream: " << getName();
        try {
            const char* q_name = getName().c_str();
            m_queue = allocate_shm<message_queue_t>(q_name);
            return true;
        } catch (bip::bad_alloc &e) {
            BOOST_LOG_TRIVIAL(error) << "error while allocating ShmStream: " << getName() << " - " << e.what();
        }
        return false;
    }

    virtual bool deallocate() {
        if (m_queue) {
            deallocate_shm<message_queue_t>(m_queue);
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
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
    message_queue_t* m_queue;
};


template<typename T, size_t N>
class ShmBufferedStream : public ShmStreamBase {

    typedef bip::allocator<T, bip::managed_shared_memory::segment_manager> queue_allocator;
    typedef blf::spsc_queue<T, blf::capacity<N> > message_queue_t;
    typedef ipdf::ShmBufferPool< ipdf::ShmBufferRef<T>, N > pool_t;

public:

    ShmBufferedStream(const char* channel_name, const char* stream_name, unsigned int buffer_size, const std::shared_ptr<bip::managed_shared_memory>& msm) :
            ShmStreamBase(channel_name, stream_name, msm),
            m_memory_manager(msm),
            m_buffer_size(buffer_size),
            m_pool(nullptr),
            m_queue(nullptr)
    { }

    virtual bool allocate() {
        try {
            const char* q_name_s = getName().c_str();
            m_queue = allocate_shm<message_queue_t>(q_name_s);

            std::string p_name = getName() + std::string("_POOL");
            const char* p_name_s = p_name.c_str();
            m_pool = new pool_t(p_name_s, m_buffer_size, m_memory_manager);

            return true;
        } catch (bip::bad_alloc &e) {
            BOOST_LOG_TRIVIAL(error) << "error while allocating ShmStream: " << getName() << " - " << e.what();
        }
        return false;
    }

    virtual bool deallocate() {
        if (m_queue) {
            deallocate_shm<message_queue_t>(m_queue);
            m_queue = nullptr;
            m_pool = nullptr;
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
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
    std::unique_ptr<pool_t> m_pool;
    unsigned int m_buffer_size;
    message_queue_t* m_queue;
};

} // end namespace

#endif //IPDF_SHMSTREAM_H
