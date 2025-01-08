#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "doubly_linked_list.hpp"

TEST(test_doubly_linked_list, basic) {
  DoublyLinkedList<int> dll;
  EXPECT_EQ(dll.GetSize(), 0);

  const int N = 10;
  LinkedListElement<int> e[N];

  // test append
  for (int i = 0; i < N; ++i) {
    e[i].data_ = i;
    auto err = dll.Append(&e[i]);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(dll.GetSize(), i + 1);
  }
  EXPECT_EQ(dll.GetSize(), N);
  EXPECT_EQ(dll.GetHead()->data_, 0);
  EXPECT_EQ(dll.GetTail()->data_, N - 1);

  // test search and remove
  for (int i = 0; i < N; ++i) {
    auto ptr = dll.Search(i);
    EXPECT_NE(ptr, nullptr);
    auto err = dll.Remove(ptr);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(dll.GetSize(), N - i - 1);
  }
  EXPECT_EQ(dll.GetSize(), 0);

  // test insert
  for (int i = 0; i < N; ++i) {
    auto anchor = dll.GetHead();
    auto err = dll.Insert(anchor, &e[i]);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(dll.GetSize(), i + 1);
  }
  EXPECT_EQ(dll.GetSize(), N);
  EXPECT_EQ(dll.GetHead()->data_, N - 1);
  EXPECT_EQ(dll.GetTail()->data_, 0);

  for (int i = 0; i < N; ++i) {
    auto ptr = dll.Get(i);
    EXPECT_EQ(ptr->data_, N - i - 1);
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}