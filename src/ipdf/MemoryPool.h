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

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#endif


namespace boost    {
namespace lockfree {
namespace detail   {

typedef parameter::parameters<boost::parameter::optional<tag::allocator>,
                              boost::parameter::optional<tag::capacity>
> memorypool_signature;

} /* namespace detail */


/** The memory pool manager combines a lockfree queue and a stack in order to manage and transfer memory segment references
 *
 *  \b Policies:
 *  - \ref boost::lockfree::fixed_sized, defaults to \c boost::lockfree::fixed_sized<false> \n
 *    Can be used to completely disable dynamic memory allocations during push in order to ensure lockfree behavior. \n
 *    If the data structure is configured as fixed-sized, the internal nodes are stored inside an array and they are addressed
 *    by array indexing. This limits the possible size of the memorypool to the number of elements that can be addressed by the index
 *    type (usually 2**16-2), but on platforms that lack double-width compare-and-exchange instructions, this is the best way
 *    to achieve lock-freedom.
 *
 *  - \ref boost::lockfree::capacity, optional \n
 *    If this template argument is passed to the options, the size of the memorypool is set at compile-time.\n
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
#ifndef BOOST_DOXYGEN_INVOKED
template <typename T,
        class A0 = boost::parameter::void_,
        class A1 = boost::parameter::void_,
        class A2 = boost::parameter::void_>
#else
template <typename T, ...Options>
#endif
class memorypool
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

    typedef typename detail::memorypool_signature::bind<A0, A1, A2>::type bound_args;

    static const bool has_capacity = detail::extract_capacity<bound_args>::has_capacity;
    static const size_t capacity = detail::extract_capacity<bound_args>::capacity;
    static const bool fixed_sized = detail::extract_fixed_sized<bound_args>::value;
    static const bool node_based = !(has_capacity || fixed_sized);
    static const bool compile_time_sized = has_capacity;

    typedef typename detail::extract_allocator<bound_args, T>::type node_allocator;


    typedef blf::queue<T, blf::capacity<capacity> > queue_t;
    typedef blf::stack<T, blf::capacity<capacity> > stack_t;

    void initialize(void)
    {
        for (unsigned int i=0; i < capacity; i++) {
            m_stack.push(T());
        }
    }

    struct implementation_defined
    {
      typedef node_allocator allocator;
      typedef std::size_t size_type;
    };

#endif

#ifndef BOOST_NO_CXX11_DELETED_FUNCTIONS
    memorypool(memorypool const &) = delete;
    memorypool(memorypool &&)      = delete;
    const memorypool& operator=( const memorypool& ) = delete;
#endif

public:
    typedef T value_type;
    typedef typename implementation_defined::allocator allocator;
    typedef typename implementation_defined::size_type size_type;

    /**
     * \return true, if implementation is lock-free.
     *
     * \warning It only checks, if the memorypool the head node and the freelist can be modified in a lock-free manner.
     *       On most platforms, the whole implementation is lock-free, if this is true. Using c++0x-style atomics, there is
     *       no possibility to provide a completely accurate implementation, because one would need to test every internal
     *       node, which is impossible if further nodes will be allocated from the operating system.
     * */
    bool is_lock_free (void) const
    {
        return m_queue.is_lock_free() && m_stack.is_lock_free();
    }

    //! Construct queue
    // @{
    memorypool(void)
    {
        BOOST_ASSERT(has_capacity);
        initialize();
    }

    template <typename U>
    explicit memorypool(typename node_allocator::template rebind<U>::other const & alloc):
            m_queue(alloc),
            m_stack(alloc)
    {
        BOOST_STATIC_ASSERT(has_capacity);
        initialize();
    }

    explicit memorypool(allocator const & alloc):
            m_queue(alloc),
            m_stack(alloc)
    {
        BOOST_ASSERT(has_capacity);
        initialize();
    }
    // @}

    //! Construct pool, allocate n nodes for the freelist.
    // @{
    explicit memorypool(size_type n):
            m_queue(n),
            m_stack(n)
    {
        BOOST_ASSERT(!has_capacity);
        initialize();
    }

    template <typename U>
    memorypool(size_type n, typename node_allocator::template rebind<U>::other const & alloc):
            m_queue(n, alloc),
            m_stack(n, alloc)
    {
        BOOST_STATIC_ASSERT(!has_capacity);
        initialize();
    }
    // @}

    /** \copydoc boost::lockfree::stack::reserve
     * */
    void reserve(size_type n)
    {
        m_queue.reserve(n);
        m_stack.reserve(n);
    }

    /** \copydoc boost::lockfree::stack::reserve_unsafe
     * */
    void reserve_unsafe(size_type n)
    {
        m_queue.reserve_unsafe(n);
        m_stack.reserve_unsafe(n);
    }

    /** Destroys queue, free all nodes from freelist.
     * */
    ~memorypool(void)
    {
        // anything needs to be done for cleanup ??
    }

    /** Obtain object from pool.
     *
     * \returns object pointer or NULL, if the pool is empty.
     *
     * \note Thread-safe. If internal memory pool is exhausted and the memory pool is not fixed-sized, a new node will be allocated
     *                    from the OS. This may not be lock-free.
     * */
    bool obtain(T & buf)
    {
        return m_stack.pop(buf);
    }


    /** Releases object to the pool.
     *
     * \returns true, if the release operation is successful, false on error.
     *
     * \note Thread-safe and non-blocking
     * */
    bool release(const T& buf)
    {
        return m_stack.push(buf);
    }


    bool send(const T& buf) {
        return m_queue.push(buf);
    }

    bool receive(T& buf) {
        return m_queue.pop(buf);
    }
private:
#ifndef BOOST_DOXYGEN_INVOKED
    queue_t m_queue;
    stack_t m_stack;
#endif
};

} /* namespace lockfree */
} /* namespace boost */

#if defined(_MSC_VER)
#pragma warning(pop)
#endif


#endif //IPDF_MEMORYPOOL_H_H
