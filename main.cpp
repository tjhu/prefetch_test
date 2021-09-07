#include <x86intrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include <string>
#include <iostream>
#include <span>
#include <array>
#include <memory>

#include "wyhash.h"

typedef void(*prefetch_fn_t)(const void*);

constexpr uint64_t CACHE_BLOCK_BITS = 6;
constexpr uint64_t CACHE_BLOCK_MASK = (1ULL << CACHE_BLOCK_BITS) - 1;

// Address of 64 byte block brought into the cache when ADDR accessed
constexpr uint64_t cache_block_aligned_addr(uint64_t addr) {
  return addr & ~CACHE_BLOCK_MASK;
}

void noop(const void*) {}

void builtin_prefetch_r(const void* data) {
  __builtin_prefetch(data, false, 3);
}

void builtin_prefetch_w(const void* data) {
  __builtin_prefetch(data, true, 3);
}

void mm_prefetch(const void* data) {
  _mm_prefetch(data, _MM_HINT_T0);
}

void benchmark(const std::string& name, prefetch_fn_t prefetch, size_t iterations, size_t delay, const uint8_t* data, const uint64_t size_mask) {
    unsigned int tmp;
    uint64_t seed = 0;
    const auto start = __rdtsc();
    for (size_t i = 0; i < iterations; i++) {
      const uint64_t offset = wyrand(&seed) & size_mask;
      const uint64_t aligned_offset = cache_block_aligned_addr(offset);
      prefetch(data + aligned_offset);

      for (size_t i = 0; i < delay; i++) {
        asm volatile("nop");
      }
    }
    const auto end = __rdtscp(&tmp);
    const auto duration = end - start;

    const auto test_size = size_mask + 1;
    std::cout << name << ": " << duration / test_size << " cycles" << std::endl;
}

template <size_t size>
void benchmark(const std::string& name, prefetch_fn_t prefetch, size_t iterations, size_t delay) {
  const uint64_t true_size = 1 << size;
  const auto data = new uint8_t[true_size];
  const uint64_t size_mask = true_size - 1;
  benchmark(name, prefetch, iterations, delay, data, size_mask);
  delete[] data;
}

int main() {
  const uint64_t test_size = 30;
  const uint64_t iterations = 1 << 26;

  for (uint64_t i = 0; i < 10; i++) {
    const auto delay = 1 << i;
    benchmark<test_size>("prefetch_w_" + std::to_string(delay), builtin_prefetch_w, iterations, delay);
  }

  return 0;
}