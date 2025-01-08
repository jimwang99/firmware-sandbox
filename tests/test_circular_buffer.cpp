#include <gtest/gtest.h>
#include <spdlog/cfg/env.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>

#include "circular_buffer.hpp"

const size_t kCapacity = 3;
DEFINE_STATIC_CIRCULAR_BUFFER(cb1, uint32_t, kCapacity);

TEST(test_circular_buffer, basic) {
  EXPECT_TRUE(cb1.IsEmpty());
  for (uint32_t i = 0; i < kCapacity; ++i) {
    auto err = cb1.Write(i);
    EXPECT_EQ(err, 0);
  }
  EXPECT_TRUE(cb1.IsFull());
  for (uint32_t i = 0; i < kCapacity; ++i) {
    auto err = cb1.Write(i);
    EXPECT_EQ(err, -ENOBUFS);
  }
  EXPECT_TRUE(cb1.IsFull());

  for (uint32_t i = 0; i < kCapacity; ++i) {
    uint32_t j;
    auto err = cb1.Read(j);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(i, j);
  }
  EXPECT_TRUE(cb1.IsEmpty());
  for (uint32_t i = 0; i < kCapacity; ++i) {
    uint32_t j;
    auto err = cb1.Read(j);
    EXPECT_EQ(err, -ENODATA);
  }
}

void ThreadProducer() {
  spdlog::info("ThreadProducer starts ...");
  EXPECT_TRUE(cb1.IsEmpty());
  for (uint32_t i = 0; i < kCapacity * 10; ++i) {
    while (cb1.IsFull()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto err = cb1.Write(i);
    EXPECT_EQ(err, 0);
    spdlog::debug("P: Write {}", i);
  }
}

void ThreadConsumer() {
  spdlog::info("ThreadConsumer starts ...");
  for (uint32_t i = 0; i < kCapacity * 10; ++i) {
    uint32_t j;
    while (cb1.IsEmpty()) {
      std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    auto err = cb1.Read(j);
    EXPECT_EQ(err, 0);
    EXPECT_EQ(i, j);
    spdlog::debug("C: Read {}", j);
  }
}

TEST(test_circular_buffer, multithread) {
  EXPECT_TRUE(cb1.IsEmpty());
  std::thread tp(ThreadProducer);
  std::thread tc(ThreadConsumer);
  tp.join();
  tc.join();
  EXPECT_TRUE(cb1.IsEmpty());
}

int main(int argc, char **argv) {
  spdlog::cfg::load_env_levels();
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
