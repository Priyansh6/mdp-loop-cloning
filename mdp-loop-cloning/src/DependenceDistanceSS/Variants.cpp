#include "DependenceDistanceSS/Variants.h"
#include "Utils.h"

#include <cstddef>

extern void potentially_modify_memory(int* a);

int test_base(int* a, unsigned int distance, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    if (i % distance == 0) { a[slow_identity(i)] = 2; }
    potentially_modify_memory(a);// This is done to ensure the compiler does not forward this store to the load below
    sum += a[i];
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
    } else {
      potentially_modify_memory2(a);
      sum += a[i];
    }
  }
  return sum;
}