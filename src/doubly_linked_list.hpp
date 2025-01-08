#pragma once

#include <errno.h>

template <typename T>
class LinkedListElement {
 public:
  T data_;
  LinkedListElement<T>* prev_;
  LinkedListElement<T>* next_;

  LinkedListElement() : prev_(nullptr), next_(nullptr) {}

  bool operator==(const LinkedListElement<T>& other) {
    return data_ == other.data_;
  }
};

template <typename T>
class DoublyLinkedList {
 private:
  LinkedListElement<T>* head_;
  LinkedListElement<T>* tail_;
  size_t size_;

 public:
  DoublyLinkedList() : head_(nullptr), tail_(nullptr), size_(0) {}

  size_t GetSize() const noexcept { return size_; }
  LinkedListElement<T>* GetHead() const noexcept { return head_; }
  LinkedListElement<T>* GetTail() const noexcept { return tail_; }

  bool IsIn(LinkedListElement<T>* elem) const noexcept {
    if (GetSize() == 0) {
      return false;
    }
    auto e = head_;
    while (e != nullptr) {
      if (e == elem) {
        return true;
      }
      e = e->next_;
    }
    return false;
  }

  int Insert(LinkedListElement<T>* anchor,
             LinkedListElement<T>* elem) noexcept {
    // when this list is empty
    if (anchor == nullptr) {
      assert(GetSize() == 0);
      head_ = elem;
      tail_ = elem;
      elem->prev_ = nullptr;
      elem->next_ = nullptr;
      size_ = 1;
      return 0;
    }
    // make sure anchor is inside this doubly-linked-list
    assert(IsIn(anchor));
    // update elem
    elem->prev_ = anchor->prev_;
    elem->next_ = anchor;
    // update anchor
    anchor->prev_ = elem;
    // update anchor's prev
    if (elem->prev_ != nullptr) {
      elem->prev_->next_ = elem;
    } else {
      assert(anchor == head_);
      head_ = elem;
    }

    size_++;
    return 0;
  }

  int Append(LinkedListElement<T>* elem) noexcept {
    if (GetSize() == 0) {
      head_ = elem;
      tail_ = elem;
      elem->prev_ = nullptr;
      elem->next_ = nullptr;
      size_ = 1;
      return 0;
    }
    elem->prev_ = tail_;
    elem->next_ = nullptr;
    tail_->next_ = elem;
    tail_ = elem;
    size_++;
    return 0;
  }

  int Remove(LinkedListElement<T>* elem) noexcept {
    if (!IsIn(elem)) {
      return -EINVAL;
    }
    if (GetSize() == 1) {
      head_ = nullptr;
      tail_ = nullptr;
      size_ = 0;
      return 0;
    }
    auto p = elem->prev_;
    auto n = elem->next_;
    // change prev
    if (p != nullptr) {
      p->next_ = n;
    } else {
      assert(elem == head_);
      head_ = n;
    }
    // change next
    if (n != nullptr) {
      n->prev_ = p;
    } else {
      assert(elem == tail_);
      tail_ = p;
    }
    elem->prev_ = nullptr;
    elem->next_ = nullptr;
    size_--;
    return 0;
  }

  LinkedListElement<T>* Search(T& data) const noexcept {
    if (GetSize() == 0) {
      return nullptr;
    }
    auto e = head_;
    while (e != nullptr) {
      if (e->data_ == data) {
        return e;
      }
      e = e->next_;
    }

    return nullptr;
  }

  LinkedListElement<T>* Get(size_t index) const noexcept {
    if (index >= GetSize()) {
      return nullptr;
    }
    auto n = head_;
    for (size_t i = 1; i <= index; ++i) {
      n = n->next_;
    }
    return n;
  }
};
