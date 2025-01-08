#pragma once

#include <stddef.h>
#include <stdint.h>

#include <type_traits>

#include "doubly_linked_list.hpp"
#include "object_pool.hpp"

#define DEFINE_STATIC_HASH_MAP(NAME, KEY_TYPE, VALUE_TYPE, CAPACITY)          \
  LinkedListElement<KeyValuePair<KEY_TYPE, VALUE_TYPE>>                       \
      __hash_map_pool_buffer_##NAME[CAPACITY];                                \
  ObjectPool<LinkedListElement<KeyValuePair<KEY_TYPE, VALUE_TYPE>>, CAPACITY> \
      __hash_map_pool_##NAME(__hash_map_pool_buffer_##NAME);                  \
  HashMap<KEY_TYPE, VALUE_TYPE, CAPACITY> NAME(&__hash_map_pool_##NAME);

template <typename TK, typename TV>
class KeyValuePair {
 public:
  TK key_;
  TV value_;

  bool operator==(const KeyValuePair<TK, TV>& other) {
    return key_ == other.key_;
  }
};

template <typename TK, typename TV, size_t CAPACITY,
          size_t (*HASH_FUNC_POINTER)(TK) = nullptr>
class HashMap {
 private:
  DoublyLinkedList<KeyValuePair<TK, TV>>
      dll_array_[CAPACITY];  // an array of DoublyLinkedList of KeyValuePair.
                             // Since the linked list object itself doesn't
                             // require much space, it could stay on the stack.
  ObjectPool<LinkedListElement<KeyValuePair<TK, TV>>, CAPACITY>*
      lle_pool_;  // point to object pool of KeyValuePair for the
                  // DoublyLinkedList, and object pool must be instantiated
                  // statically because each element stores the both key and
                  // value which could take a lot of space.
  size_t size_;   // totally number of elements

  size_t KeyToIndex(const TK& key) const noexcept {
    // Calls HASH_FUNC_POINTER to map from TK (key) to size_t (index)
    // if TK is already integral, HASH_FUNC_POINTER can be nullptr
    if (HASH_FUNC_POINTER == nullptr) {
      static_assert(std::is_integral_v<TK>);
      return static_cast<size_t>(key) % CAPACITY;
    } else {
      return HASH_FUNC_POINTER(key) % CAPACITY;
    }
  }

  LinkedListElement<KeyValuePair<TK, TV>>* Find(const TK& key) const noexcept {
    auto idx = KeyToIndex(key);
    KeyValuePair<TK, TV> kv;
    kv.key_ = key;
    return dll_array_[idx].Search(kv);
  }

 public:
  HashMap(
      ObjectPool<LinkedListElement<KeyValuePair<TK, TV>>, CAPACITY>* const pool)
      : lle_pool_(pool), size_(0) {}

  size_t GetSize() { return size_; }

  int Set(const TK& key, const TK& value) noexcept {
    auto lle = Find(key);
    if (lle == nullptr) {
      // allocate an new object from object pool
      lle = lle_pool_->Alloc();
      assert(lle != nullptr);

      lle->data_.key_ = key;
      lle->data_.value_ = value;
      lle->prev_ = nullptr;
      lle->next_ = nullptr;

      // find the dll, and append to it
      auto idx = KeyToIndex(key);
      dll_array_[idx].Append(lle);
      size_++;
      return 0;
    }
    // update existing object
    memcpy(&lle->data_.value_, &value, sizeof(TK));
    return 0;
  }

  int Get(const TK& key, TV& value) const noexcept {
    auto lle = Find(key);
    if (lle == nullptr) {
      return -ENODATA;
    }
    memcpy(&value, &lle->data_.value_, sizeof(TV));
    return 0;
  }

  int Remove(const TK& key) noexcept {
    auto lle = Find(key);
    if (lle == nullptr) {
      return -ENODATA;
    }
    auto idx = KeyToIndex(key);
    dll_array_[idx].Remove(lle);
    lle_pool_->Free(lle);
    size_--;
    return 0;
  }
};