#include "DependenceDistance/Variants.h"
#include "Utils.h"

#include <cstddef>

#define EXTRA_LONG_LOOP 1

volatile int x{ 0 };
volatile int y{ 0 };

int test_base(int* a, unsigned int distance, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    if (i % distance == 0) { a[slow_identity(i)] = 2; }
    potentially_modify_memory(a);// This is done to ensure the compiler does not forward this store to the load below
    sum += a[i];
#if EXTRA_LONG_LOOP
    sum += computeComplexSum(x, y);
#endif
  }
  return sum;
}

int test_cloned_load(int* a, unsigned int distance, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    if (i % distance == 0) { a[slow_identity(i)] = 2; }
    if (i % distance == 0) {
      potentially_modify_memory(a);
      sum += a[i];
#if EXTRA_LONG_LOOP
      sum += computeComplexSum(x, y);
#endif
    } else {
      potentially_modify_memory2(a);
      sum += a[i];
#if EXTRA_LONG_LOOP
      sum += computeComplexSum(x, y);
#endif
    }
  }
  return sum;
}