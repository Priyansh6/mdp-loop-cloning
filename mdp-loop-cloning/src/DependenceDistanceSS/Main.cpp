#include "DependenceDistanceSS/Variants.h"

#include <array>
#include <cstddef>

#define CLONE 1

constexpr std::size_t N = 1000000;
constexpr unsigned int distance = 2;

std::array<int, N> l;

// Simple example to demonstrate potential gains for Store Sets by cloning simple loops containing a store and a load
// All performance gains are due to a large reduction in false dependencies between potentially aliasing stores and loads
int main()
{
  int sum = 0;

#if CLONE
  sum += test_aliases_cloned(&l.front(), distance, N);
#else
  sum += test_aliases(&l.front(), distance, N);
#endif

  return sum;
}