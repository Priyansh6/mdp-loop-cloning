#include "LoopClone/Variants.h"

#include <array>
#include <cstddef>

#define OVERLAP 1
#define CLONE 1
#define OVERLAP_SIZE 3

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
#if OVERLAP
  for (int i = 0; i < num_iters; i++) {
    // sum += test_cloned_loop(&l.front(), &l.front() + OVERLAP_SIZE, 10);
    // sum += test_cloned_loop(&l.front(), &k.front(), N);
    sum += test_cloned_loop_with_overlap_check(&l.front(), &l.front() + OVERLAP_SIZE, 10);
    sum += test_cloned_loop_with_overlap_check(&l.front(), &k.front(), N);
  }
#else
  for (int i = 0; i < num_iters; i++) {
    sum += test_cloned_loop(&l.front(), &l.front(), 10);
    sum += test_cloned_loop(&l.front(), &k.front(), N);
  }
#endif
#else
#if OVERLAP
  for (int i = 0; i < num_iters; i++) {
    sum += test_base(&l.front(), &l.front() + OVERLAP_SIZE, 10);
    sum += test_base(&l.front(), &k.front(), N);
  }
#else
  for (int i = 0; i < num_iters; i++) {
    sum += test_base(&l.front(), &l.front(), 10);
    sum += test_base(&l.front(), &k.front(), N);
  }
#endif
#endif

  return sum;
}