#include <gtest/gtest.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>

#include "object_pool.hpp"

const size_t kCapacity = 3;
DEFINE_STATIC_OBJECT_POOL(op1, int, kCapacity);
DEFINE_STATIC_STACK(st1, int *, kCapacity);

TEST(test_object_pool, basic) {
  EXPECT_TRUE(op1.IsEmpty());
  for (uint32_t i = 0; i < kCapacity; ++i) {
    auto ptr = op1.Alloc();
    st1.Push(ptr);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(op1.GetErrorCode(), 0);
  }
  EXPECT_TRUE(op1.IsFull());
  for (uint32_t i = 0; i < kCapacity; ++i) {
    auto ptr = op1.Alloc();
    EXPECT_EQ(ptr, nullptr);
    EXPECT_EQ(op1.GetErrorCode(), -ENOBUFS);
  }
  EXPECT_TRUE(op1.IsFull());

  for (uint32_t i = 0; i < kCapacity; ++i) {
    int *ptr;
    st1.Pop(ptr);
    auto e = op1.Free(ptr);
    EXPECT_EQ(e, 0);
  }
  EXPECT_TRUE(op1.IsEmpty());

  EXPECT_TRUE(op1.IsEmpty());
  for (uint32_t i = 0; i < kCapacity; ++i) {
    auto ptr = op1.Alloc();
    st1.Push(ptr);
    EXPECT_NE(ptr, nullptr);
    EXPECT_EQ(op1.GetErrorCode(), 0);
  }
  EXPECT_TRUE(op1.IsFull());
}

int main(int argc, char **argv) {
  spdlog::cfg::load_env_levels();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
