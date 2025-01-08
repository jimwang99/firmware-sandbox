#pragma once

template <typename T>
class BinarySearchTreeElement {
 public:
  T* data_;
  BinarySearchTreeElement<T>* parent_;
  BinarySearchTreeElement<T>* child_[2];
  BinarySearchTreeElement()
      : parent_(nullptr), child_({nullptr, nullptr}), data_(nullptr) {}

  bool operator==(const BinarySearchTreeElement<T>* other) {
    return (*data_ == *other);
  }
};

template <typename T>
class BinarySearchTree {
 private:
  BinarySearchTreeElement<T>* root_;
  size_t size_;

 public:
  BinarySearchTree() : root_(nullptr), size_(0) {}
  int Insert(BinarySearchTreeElement<T>* elem) { return 0; }
  int Remove(BinarySearchTreeElement<T>* elem) { return 0; }
};