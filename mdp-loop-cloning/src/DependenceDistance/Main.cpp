#include "DependenceDistance/Variants.h"

#include <array>
#include <cstddef>

#define CLONE 0

constexpr std::size_t N = 1000000;
constexpr unsigned int distance = 2;

std::array<int, N> l;

/* Simple example to demonstrate potential gains for Store Sets by cloning stores and loads depending on */
/* the distance (number of iterations) between which these are dependent.*/
/* Gains are due to a large reduction in false dependencies between potentially aliasing stores and loads.*/
/* Assumes addresses are compared exactly (i.e. LSQDepCheckShift set to 0 in gem5/src/cpu/o3/BaseCPUO3.py)*/
int main()
{
  int sum = 0;

#if CLONE
  sum += test_cloned_load(&l.front(), distance, N);
#else
  sum += test_base(&l.front(), distance, N);
#endif

  return sum;
}