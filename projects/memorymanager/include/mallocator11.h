#ifndef MALLOCATOR11_H
#define MALLOCATOR11_H

#include <utility>
#include <cstdlib>
#include <new>

template<class T>
struct Mallocator11 {
  using value_type = T;
  using pointer = T*;
  using propagate_on_container_copy_assignment = std::true_type;
  using propagate_on_container_move_assignment = std::true_type;
  using propagate_on_container_swap = std::true_type;

  Mallocator11(Mallocator11 const&) = default;
  Mallocator11& operator=(Mallocator11 const&) = default;
  Mallocator11()=default;
  template<class U>
  Mallocator11(Mallocator11<U> const&) noexcept {}
  template<class U>
  Mallocator11& operator=(Mallocator11<U> const&) noexcept {return *this;}


  pointer allocate(std::size_t n) {
    if (std::size_t(-1) / sizeof(T) < n)
      throw std::bad_array_new_length(); // or something else
    if (!n) return nullptr; // zero means null, not throw
    if(auto*r= static_cast<pointer>(malloc(n * sizeof(T))))
      return r;
    throw std::bad_alloc();
  }
  void deallocate(pointer p, std::size_t n) {
    free(p);
  }
  template<class U>
  bool operator==(Mallocator11<U> const& rhs) const {
    return true;
  }
  template<class U>
  bool operator!=(Mallocator11<U> const& rhs) const {
    return false;
  }
};

#endif // MALLOCATOR11_H
