//
// Created by Ulrich Eck on 2/11/2015.
//

#ifndef IPDF_MEMORYPOOL_H_H
#define IPDF_MEMORYPOOL_H_H

#include <boost/assert.hpp>
#ifdef BOOST_NO_CXX11_DELETED_FUNCTIONS
#include <boost/noncopyable.hpp>
#endif
#include <boost/static_assert.hpp>
#include <boost/type_traits/has_trivial_assign.hpp>
#include <boost/type_traits/has_trivial_destructor.hpp>
#include <boost/lockfree/detail/parameter.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/stack.hpp>

#include <memory>

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#endif



namespace ipdf {

/* Example of ReleasePool implementation for C++14 .. needs refactoring...
#include <algorithm>

class Timer {
protected:
    virtual void startTimer(unsigned int frequency) {}
    virtual void timerCallback() {}
};

class ReleasePool : private Timer
{
public:
    ReleasePool() { startTimer (1000); }

    template<typename T> void add (const std::shared_ptr<T>& object) {
        if (object.empty())
            return;
        std::lock_guard<std::mutex> lock (m);
        pool.emplace_back (object);
    }
private:
    void timerCallback() override {
        std::lock_guard<std::mutex> lock (m);
        pool.erase(
            std::remove_if (
                pool.begin(), pool.end(),
            [] (auto& object) { return object.use_count() <= 1; } ),
            pool.end()
        );
    }

    std::vector<std::shared_ptr<void>> pool;
    std::mutex m;
};
*/


/* The memory pool manager uses a lockfree stack to manage memory segment references
 */
template <typename T, size_t capacity>
class ShmBufferPool
#ifdef BOOST_NO_CXX11_DELETED_FUNCTIONS
    : boost::noncopyable
#endif
{
private:
#ifndef BOOST_DOXYGEN_INVOKED

#ifdef BOOST_HAS_TRIVIAL_DESTRUCTOR
    BOOST_STATIC_ASSERT((boost::has_trivial_destructor<T>::value));
#endif

#ifdef BOOST_HAS_TRIVIAL_ASSIGN
    BOOST_STATIC_ASSERT((boost::has_trivial_assign<T>::value));
#endif

    // allocators ??
    typedef blf::stack<T, blf::capacity<capacity> > stack_t;

    struct implementation_defined
    {
      typedef std::size_t size_type;
    };

#endif

#ifndef BOOST_NO_CXX11_DELETED_FUNCTIONS
    ShmBufferPool(ShmBufferPool const &) = delete;
    ShmBufferPool(ShmBufferPool &&)      = delete;
    const ShmBufferPool& operator=( const ShmBufferPool& ) = delete;
#endif

public:
    typedef T value_type;
    typedef typename implementation_defined::size_type size_type;


    //! Construct queue
    // @{
    ShmBufferPool(const char* name, size_type buffer_size, const std::shared_ptr<bip::managed_shared_memory>& msm)
    : m_pool_name(name)
    , m_buffer_size(buffer_size)
    , m_memory_manager(msm)
    , m_stack(nullptr)
    {
        BOOST_LOG_TRIVIAL(info) << "ShmBufferPool constructor";
    }

    // @}

    bool allocate() {
        if (!m_stack) {
            BOOST_LOG_TRIVIAL(info) << "allocate ShmBufferPool";
            try {
                m_stack = m_memory_manager->find_or_construct<stack_t>(m_pool_name)();
            } catch (bip::bad_alloc &e) {
                BOOST_LOG_TRIVIAL(error) << "Error during shm allocation: " << e.what();
                return false;
            }
            for (unsigned int i=0; i < capacity; i++) {
                T buf;
                buf.prepare(m_buffer_size, *m_memory_manager);
                m_stack->push(buf);
            }
        }
        return true;
    }

    bool deallocate () {
        if (m_stack) {
            BOOST_LOG_TRIVIAL(info) << "deallocate ShmBufferPool";
            T buf;
            while (m_stack->pop(buf)) {
                if (buf.is_allocated()) {
                    buf.release(*m_memory_manager);
                }
            }
            m_memory_manager->destroy_ptr(m_stack);
            m_stack = nullptr;
        }
        return true;
    }

    /** Destroys queue, free all nodes from freelist.
     * */
    ~ShmBufferPool(void)
    {
        BOOST_LOG_TRIVIAL(info) << "ShmBufferPool deconstructor";
        // anything needs to be done for cleanup ??
        m_memory_manager.reset();
    }

    /** Obtain object from pool.
     *
     * \returns object pointer or NULL, if the pool is empty.
     *
     * \note Thread-safe. If internal memory pool is exhausted and the memory pool is not fixed-sized, a new node will be allocated
     *                    from the OS. This may not be lock-free.
     * */
    bool obtain(T & buf, size_t size)
    {
        bool ret  = m_stack->pop(buf);
        if (ret) {
            // XXX this will lock during allocation !!!
            return buf.prepare(size, *m_memory_manager);
        }
        return ret;
    }


    /** Releases object to the pool.
     *
     * \returns true, if the release operation is successful, false on error.
     *
     * \note Thread-safe and non-blocking
     * */
    bool release(const T& buf)
    {
        return m_stack->push(buf);
    }


private:
#ifndef BOOST_DOXYGEN_INVOKED
    const char * m_pool_name;
    size_t m_buffer_size;
    stack_t * m_stack;
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
#endif
};

} // end namespace ipdf

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif //IPDF_MEMORYPOOL_H_H
