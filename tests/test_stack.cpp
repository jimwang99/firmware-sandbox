#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include "stack.hpp"

const size_t kCapacity = 3;
DEFINE_STATIC_STACK(stack1, int, kCapacity);

TEST(test_stack, basic) {
  EXPECT_EQ(stack1.GetCapacity(), kCapacity);
  EXPECT_TRUE(stack1.IsEmpty());
  EXPECT_FALSE(stack1.IsFull());

  for (int i = 0; i < kCapacity; ++i) {
    auto err = stack1.Push(i);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(stack1.GetSize(), i + 1);
    EXPECT_FALSE(stack1.IsEmpty());
  }
  EXPECT_TRUE(stack1.IsFull());
  for (int i = 0; i < kCapacity; ++i) {
    auto err = stack1.Push(i);
    EXPECT_EQ(err, -ENOBUFS);
    EXPECT_EQ(stack1.GetSize(), kCapacity);
    EXPECT_TRUE(stack1.IsFull());
  }

  for (int j = kCapacity - 1; j >= 0; --j) {
    int i;
    auto err = stack1.Pop(i);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(i, j);
    EXPECT_EQ(stack1.GetSize(), j);
    EXPECT_FALSE(stack1.IsFull());
  }
  EXPECT_TRUE(stack1.IsEmpty());
  for (int j = 0; j < kCapacity; ++j) {
    int i;
    auto err = stack1.Pop(i);
    EXPECT_EQ(err, -ENODATA);
    EXPECT_TRUE(stack1.IsEmpty());
  }
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}