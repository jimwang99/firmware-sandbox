#include <gtest/gtest.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include "hash_map.hpp"

const size_t kCap = 3;
DEFINE_STATIC_HASH_MAP(hash1, int, int, kCap);

TEST(test_hash_map, basic) {
  EXPECT_EQ(hash1.GetSize(), 0);
  for (int i = 0; i < kCap; ++i) {
    hash1.Set(i, i * 2);
    EXPECT_EQ(hash1.GetSize(), i + 1);
  }
  for (int i = 0; i < kCap; ++i) {
    int j = -1;
    auto err = hash1.Get(i, j);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(j, i * 2);
  }
  {
    int j = -1;
    auto err = hash1.Get(kCap + 1, j);
    EXPECT_EQ(err, -ENODATA);
  }
  {
    auto err = hash1.Remove(kCap + 1);
    EXPECT_EQ(err, -ENODATA);
  }
  for (int i = 0; i < kCap; ++i) {
    auto err = hash1.Remove(i);
    EXPECT_EQ(err, 0);
  }
  EXPECT_EQ(hash1.GetSize(), 0);
}

int main(int argc, char **argv) {
  spdlog::cfg::load_env_levels();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
