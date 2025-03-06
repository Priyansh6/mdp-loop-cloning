#include "SimpleSS/Variants.h"

#include <array>
#include <cstddef>

#define CLONE 0

constexpr int num_iters = 100;
constexpr std::size_t N = 10000;

std::array<int, N> l;
std::array<int, N> k;

/* Simple example to demonstrate potential gains for Store Sets by cloning simple loops containing a store and */
/* a load. Gains are due to a large reduction in false dependencies between potentially aliasing stores and loads. */
int main()
{
  int sum = 0;

#if CLONE
  for (int i = 0; i < num_iters; i++) {
    sum += test_cloned_loop(&l.front(), &l.front(), 10);
    sum += test_cloned_loop(&l.front(), &k.front(), N);
  }
#else
  for (int i = 0; i < num_iters; i++) {
    sum += test_base(&l.front(), &l.front(), 10);
    sum += test_base(&l.front(), &k.front(), N);
  }
#endif

  return sum;
}