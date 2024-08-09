#pragma once

#include <iostream>
#include <memory>
#include <rte_malloc.h>

template <typename T> class DPDKAllocator
{
  public:
    using value_type = T;

    DPDKAllocator() noexcept = default;

    template <typename U> DPDKAllocator(const DPDKAllocator<U> &) noexcept
    {
    }

    T *allocate(std::size_t n)
    {
        // Allocate memory using rte_malloc
        T *ptr = static_cast<T *>(rte_malloc(nullptr, n * sizeof(T), 0));
        if (!ptr)
        {
            throw std::bad_alloc();
        }
        std::cout << "DPDKAllocator - Allocated " << n * sizeof(T) << " bytes" << std::endl;
        return ptr;
    }

    void deallocate(T *p, std::size_t n) noexcept
    {
        // Deallocate memory using rte_free
        rte_free(p);
        std::cout << "DPDKAllocator - Deallocated " << n * sizeof(T) << " bytes" << std::endl;
    }

    // Constructs an object of type T in allocated storage pointed by p.
    template <typename U, typename... Args> void construct(U *p, Args &&... args)
    {
        ::new ((void *)p) U(std::forward<Args>(args)...);
    }

    // Destroys an object of type T pointed by p.
    template <typename U> void destroy(U *p)
    {
        p->~U();
    }
};

// Comparators for checking equality between different instances of the allocator
template <typename T, typename U> bool operator==(const DPDKAllocator<T> &, const DPDKAllocator<U> &) noexcept
{
    return true;
}

template <typename T, typename U> bool operator!=(const DPDKAllocator<T> &, const DPDKAllocator<U> &) noexcept
{
    return false;
}
