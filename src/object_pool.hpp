#pragma once

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "stack.hpp"

#define DEFINE_STATIC_OBJECT_POOL(NAME, TYPENAME, CAPACITY) \
  TYPENAME __object_pool_buffer_##NAME[CAPACITY];           \
  ObjectPool<TYPENAME, CAPACITY> NAME(__object_pool_buffer_##NAME);

template <typename T, size_t CAPACITY>
class ObjectPool {
 private:
  T* buffer_;
  size_t stack_buffer_[CAPACITY];
  Stack<size_t, CAPACITY> stack_;
  int err_;

 public:
  ObjectPool(T* buffer) : buffer_(buffer), stack_(stack_buffer_), err_(0) {
    // all elements are free
    for (size_t i = 0; i < CAPACITY; ++i) {
      stack_.Push(i);
    }
  }

  size_t GetCapacity() const noexcept { return CAPACITY; }
  size_t GetSize() const noexcept { return stack_.GetSize(); }
  bool IsEmpty() const noexcept { return stack_.IsFull(); }
  bool IsFull() const noexcept { return stack_.IsEmpty(); }
  int GetErrorCode() const noexcept { return err_; }

  // allocate 1 element
  T* Alloc() noexcept {
    if (IsFull()) {
      err_ = -ENOBUFS;
      return nullptr;
    }

    size_t idx;
    err_ = stack_.Pop(idx);
    assert(err_ == 0);

    return &buffer_[idx];
  }

  // allocate N elements contiguously
  T* AllocContiguous(size_t num) noexcept {
    if (num > stack_.GetSize()) {
      err_ = -ENOBUFS;
      return nullptr;
    }
    if (num == 0) {
      err_ = -EINVAL;
      return nullptr;
    }

    // create a map of empty elements
    // mark them as 1's in map[]
    uint8_t map[CAPACITY] = {0};
    T* result = nullptr;
    err_ = -ENOBUFS;
    size_t idx;
    while (!stack_.IsEmpty()) {
      // from stack_ to map
      auto e = stack_.Pop(idx);
      assert(e == 0);
      map[idx] = 1;

      // go through idx's neighours to see if they can satisfy the requirements
      // use double pointers
      size_t i = idx, j = idx;
      while ((j - i + 1) < num) {
        // pointer-i sees into the left side
        if ((i > 0) && (map[i - 1] == 1)) {
          i--;
          continue;  // found empty spot, check exit criteria
        }
        // pointer-j sess into he right side
        if ((j < (CAPACITY - 1)) && (map[j + 1] == 1)) {
          j++;
          continue;  // found empty spot, check exit criteria
        }
        break;  // neither i nor j has moved this round, break
      }
      if ((j - i + 1) == num) {
        // found enough space
        result = &buffer_[i];
        err_ = 0;
        for (int k = i; k <= j; ++k) {
          map[k] = 0;  // mark these spaces as used
        }
      }
    }
    // put all the unused space back to stack_
    for (int i = 0; i < CAPACITY; ++i) {
      if (map[i] == 1) {
        stack_.Push(i);
      }
    }
    return result;
  }

  // free 1 element
  int Free(T* ptr) noexcept {
    if (ptr < buffer_) {
      err_ = -EINVAL;
      return err_;
    }
    // calculate element index
    size_t p = reinterpret_cast<size_t>(ptr);
    size_t ofs = p - reinterpret_cast<size_t>(buffer_);
    if (ofs % sizeof(T) != 0) {
      err_ = -EINVAL;
      return err_;
    }
    size_t idx = ofs / sizeof(T);
    if (idx >= CAPACITY) {
      err_ = -EINVAL;
      return err_;
    }

    err_ = stack_.Push(idx);
    return err_;
  }
};