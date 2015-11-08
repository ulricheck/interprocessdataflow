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

/** The memory pool manager uses a lockfree stack to manage memory segment references
 *
 *  \b Policies:
 *  - \ref boost::lockfree::fixed_sized, defaults to \c boost::lockfree::fixed_sized<false> \n
 *    Can be used to completely disable dynamic memory allocations during push in order to ensure lockfree behavior. \n
 *    If the data structure is configured as fixed-sized, the internal nodes are stored inside an array and they are addressed
 *    by array indexing. This limits the possible size of the ShmBufferPool to the number of elements that can be addressed by the index
 *    type (usually 2**16-2), but on platforms that lack double-width compare-and-exchange instructions, this is the best way
 *    to achieve lock-freedom.
 *
 *  - \ref boost::lockfree::capacity, optional \n
 *    If this template argument is passed to the options, the size of the ShmBufferPool is set at compile-time.\n
 *    It this option implies \c fixed_sized<true>
 *
 *  - \ref boost::lockfree::allocator, defaults to \c boost::lockfree::allocator<std::allocator<void>> \n
 *    Specifies the allocator that is used for the internal freelist
 *
 *  \b Requirements:
 *   - T must have a copy constructor
 *   - T must have a trivial assignment operator
 *   - T must have a trivial destructor
 *
 * */
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
    : m_memory_manager(msm)
    , m_stack(msm->find_or_construct<stack_t>(name)())
    {
        BOOST_LOG_TRIVIAL(info) << "initialize ShmBufferPool";
        for (unsigned int i=0; i < capacity; i++) {
            T buf;
            buf.prepare(buffer_size, *m_memory_manager);
            m_stack->push(buf);
        }
        BOOST_LOG_TRIVIAL(info) << "initialized ShmBufferPool";
    }

    // @}


    /** \copydoc boost::lockfree::stack::reserve
     * */
    void reserve(size_type n)
    {
        m_stack->reserve(n);
    }

    /** \copydoc boost::lockfree::stack::reserve_unsafe
     * */
    void reserve_unsafe(size_type n)
    {
        m_stack->reserve_unsafe(n);
    }

    /** Destroys queue, free all nodes from freelist.
     * */
    ~ShmBufferPool(void)
    {
        BOOST_LOG_TRIVIAL(info) << "ShmBufferPool deconstructor";
        // anything needs to be done for cleanup ??
        T buf;
        while (m_stack->pop(buf)) {
            if (buf.is_allocated()) {
                buf.release(*m_memory_manager);
            }
        }
        m_memory_manager->destroy_ptr(m_stack);
        m_stack = nullptr;
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
    stack_t * m_stack;
    std::shared_ptr<bip::managed_shared_memory> m_memory_manager;
#endif
};

} // end namespace ipdf

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif //IPDF_MEMORYPOOL_H_H
