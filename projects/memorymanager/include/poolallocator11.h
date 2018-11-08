#ifndef POOLALLOCATOR11_H
#define POOLALLOCATOR11_H

#include <utility>
#include <cstdlib>
#include <new>

template<class T>
struct PoolAllocator11 {
  using value_type = T;
  using pointer = T*;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

  PoolAllocator11(PoolAllocator11 const &) = default;
  PoolAllocator11& operator=(PoolAllocator11 const&) = default;
  PoolAllocator11()=default;
  template<class U>
  PoolAllocator11(PoolAllocator11<U> const&) noexcept {}
  template<class U>
  PoolAllocator11& operator=(PoolAllocator11<U> const&) noexcept {return *this;}

  struct PoolItem {
    PoolItem* next;
  };

  PoolItem* pool_head = nullptr;

  pointer allocate(std::size_t n) {
    if (std::size_t(-1) / sizeof(T) < n)
      throw std::bad_array_new_length(); // or something else
    if (!n) return nullptr; // zero means null, not throw
    if(pool_head != nullptr) {
       if(auto*r = reinterpret_cast<pointer>(pool_head)) {
         pool_head = pool_head->next;
         return r;
       }
       throw std::bad_alloc();
    }
    if(auto*r= static_cast<pointer>(malloc(n * sizeof(T))))
      return r;
    throw std::bad_alloc();
  }

  void deallocate(pointer p, std::size_t n) {
    std::cout << "poolallocator retaining pool item!" << std::endl;
    PoolItem* pi = reinterpret_cast<PoolItem*>(p);
    pi->next = pool_head;
    pool_head = pi;
  }

  template<class U>
  bool operator==(PoolAllocator11<U> const& rhs) const {
    return true;
  }
  template<class U>
  bool operator!=(PoolAllocator11<U> const& rhs) const {
    return false;
  }
};

#endif // POOLALLOCATOR11_H
