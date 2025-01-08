#pragma once

#include <errno.h>
#include <string.h>
#include <unistd.h>

#define DEFINE_STATIC_CIRCULAR_BUFFER(NAME, TYPENAME, CAPACITY) \
  TYPENAME __circular_buffer_buffer_##NAME[CAPACITY];           \
  CircularBuffer<TYPENAME, CAPACITY> NAME(__circular_buffer_buffer_##NAME);

template <typename T, size_t CAPACITY>
class CircularBuffer {
 private:
  T *buffer_;  // buffer pointer
  size_t write_index_;
  size_t read_index_;

 public:
  CircularBuffer(T *buffer) : buffer_(buffer), write_index_(0), read_index_(0) {
    assert(buffer != nullptr);
  }

  size_t GetCapacity() const noexcept { return CAPACITY; }
  size_t GetSize() const noexcept { return write_index_ - read_index_; }
  bool IsFull() const noexcept { return GetSize() == GetCapacity(); }
  bool IsEmpty() const noexcept { return GetSize() == 0; }

  int Write(T &elem) noexcept {
    if (IsFull()) {
      return -ENOBUFS;
    }

    memcpy(&buffer_[write_index_ % CAPACITY], &elem, sizeof(T));
    write_index_++;
    return 0;
  }

  ssize_t Write(T *elem, size_t num) noexcept {
    assert(elem != nullptr);
    if ((GetCapacity() - GetSize()) < num) {
      return -ENOBUFS;
    }
    // normal multiple write
    // use single Write() multiple times to handle wrap-around
    // could be optimized for performance with 2 memcpy
    for (int i = 0; i < num; ++i) {
      auto err = Write(elem[i]);
      assert(err == 0);
    }
    return 0;
  }

  int Read(T &elem) noexcept {
    if (IsEmpty()) {
      return -ENODATA;
    }

    memmove(&elem, &buffer_[read_index_ % CAPACITY], sizeof(T));
    read_index_++;
    return 0;
  }

  ssize_t Read(T *elem, size_t num) noexcept {
    if (GetSize() < num) {
      return -ENODATA;
    }
    // move the read_index_ without memmove the data
    // use with Peek()
    if (elem == nullptr) {
      read_index_ += num;
      return num;
    }
    // normal multiple read
    // use single Read() multiple times to handle wrap-around
    // could be optimized for performance with 2 memmove
    for (int i = 0; i < num; ++i) {
      auto err = Read(elem[i]);
      assert(err == 0);
    }
    return 0;
  }

  T *Peek() noexcept {
    if (IsEmpty()) {
      return nullptr;
    }
    return &buffer_[read_index_];
  }
};
