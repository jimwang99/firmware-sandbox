#pragma once

#include <cerrno>
// #include <cstddef>

#define DEFINE_STATIC_STACK(NAME, TYPENAME, CAPACITY) \
  TYPENAME __stack_buffer_##NAME[CAPACITY];           \
  Stack<TYPENAME, CAPACITY> NAME(__stack_buffer_##NAME);

template <typename T, size_t CAPACITY>
class Stack {
 private:
  T* buffer_;
  size_t index_;

 public:
  Stack(T* buffer) : buffer_(buffer), index_(0) { assert(buffer != nullptr); }

  size_t GetCapacity() const noexcept { return CAPACITY; }
  size_t GetSize() const noexcept { return index_; }
  bool IsEmpty() const noexcept { return GetSize() == 0; }
  bool IsFull() const noexcept { return GetSize() == GetCapacity(); }

  int Push(T& elem) noexcept {
    if (IsFull()) {
      return -ENOBUFS;
    }
    memcpy(&buffer_[index_], &elem, sizeof(T));
    index_++;
    return 0;
  }

  int Pop(T& elem) noexcept {
    if (IsEmpty()) {
      return -ENODATA;
    }
    index_--;
    memcpy(&elem, &buffer_[index_], sizeof(T));
    return 0;
  }
};