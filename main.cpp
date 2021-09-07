#include <x86intrin.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include <string>
#include <iostream>
#include <span>
#include <array>

#include "wyhash.h"

typedef void(*prefetch_fn_t)(const void*);

constexpr uint64_t CACHE_BLOCK_BITS = 6;
constexpr uint64_t CACHE_BLOCK_MASK = (1ULL << CACHE_BLOCK_BITS) - 1;

// Address of 64 byte block brought into the cache when ADDR accessed
constexpr uint64_t cache_block_aligned_addr(uint64_t addr) {
  return addr & ~CACHE_BLOCK_MASK;
}

template <size_t size>
void benchmark(const std::string& name, prefetch_fn_t prefetch, size_t iterations, size_t delay) {
  const std::array<uint8_t, 1 << size> data;
  const uint64_t size_mask = ~size;
  benchmark(name, prefetch, iterations, delay, data.data, size_mask);
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
        asm("nop");
      }
    }
    const auto end = __rdtscp(&tmp);
    const auto duration = end - start;

    const auto test_size = 1 << (~size_mask);
    std::cout << name << ": " << duration / test_size << " cycles"  << std::endl;
}




int main() {



  return 0;
}