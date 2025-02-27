#include <spdlog/spdlog.h>

#include <cassert>
#include <cstdlib>

constexpr size_t CAPACITY = 1024;

uint8_t soc_memory[CAPACITY];
uint8_t cpu_memory[CAPACITY];

// from CPU memory to SOC memory
template <size_t SIZE = 8>
int write_aligned(size_t soc_addr, void* cpu_addr) {
  static_assert(SIZE == 4 || SIZE == 8);
  auto cpu_addr_int = reinterpret_cast<size_t>(cpu_addr);
  assert(soc_addr & (SIZE - 1) == 0);
  assert(cpu_addr_int & (SIZE - 1) == 0);
  for (auto i = 0; i < SIZE; ++i) {
    soc_memory[soc_addr + i] = cpu_memory[cpu_addr_int + i];
  }
}

// from SOC memory to CPU memory
template <size_t SIZE = 8>
int read_aligned(void* cpu_addr, size_t soc_addr) {
  static_assert(SIZE == 4 || SIZE == 8);
  auto cpu_addr_int = reinterpret_cast<size_t>(cpu_addr);
  assert(soc_addr & (SIZE - 1) == 0);
  assert(cpu_addr_int & (SIZE - 1) == 0);
  for (auto i = 0; i < SIZE; ++i) {
    cpu_memory[cpu_addr_int + i] = soc_memory[soc_addr + i];
  }
}

int write_flex(size_t soc_addr, void* cpu_addr, size_t sz) {
  // TODO
}

int read_flex(void* cpu_Addr, size_t soc_addr, size_t sz) {
  // TODO
}

int main() {
  for (auto i = 0; i < CAPACITY; ++i) {
    soc_memory[i] = i % 256;
    cpu_memory[i] = 0;
  }
  read_flex(&cpu_memory[0], 0, 16);    // aligned read
  write_flex(16, &cpu_memory[0], 16);  // aligned write
  for (auto i = 0; i < 16; ++i) {
    assert(cpu_memory[i] == i);
    assert(soc_memory[i] == soc_memory[i + 16]);
  }

  for (auto i = 0; i < CAPACITY; ++i) {
    soc_memory[i] = i % 256;
    cpu_memory[i] = 0;
  }
  read_flex(&cpu_memory[1], 0, 16);    // aligned -> unaligned read
  write_flex(16, &cpu_memory[1], 16);  // unaligned -> aligned write
  for (auto i = 0; i < 16; ++i) {
    assert(cpu_memory[i + 1] == i);
    assert(soc_memory[i] == soc_memory[i + 16]);
  }

  for (auto i = 0; i < CAPACITY; ++i) {
    soc_memory[i] = i % 256;
    cpu_memory[i] = 0;
  }
  read(&cpu_memory[0], 1, 16);    // unaligned -> aligned read
  write(17, &cpu_memory[0], 16);  // aligned -> unaligned write
  for (auto i = 0; i < 16; ++i) {
    assert(cpu_memory[i] == i);
    assert(soc_memory[i + 1] == soc_memory[i + 17]);
  }

  for (auto i = 0; i < CAPACITY; ++i) {
    soc_memory[i] = i % 256;
    cpu_memory[i] = 0;
  }
  read_flex(&cpu_memory[1], 1, 16);    // unaligned -> unaligned read
  write_flex(17, &cpu_memory[1], 16);  // unaligned -> unaligned write
  for (auto i = 0; i < 16; ++i) {
    assert(cpu_memory[i + 1] == i);
    assert(soc_memory[i + 1] == soc_memory[i + 17]);
  }
}