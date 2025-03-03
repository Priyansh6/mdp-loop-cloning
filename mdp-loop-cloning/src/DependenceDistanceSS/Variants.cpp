#include "DependenceDistanceSS/Variants.h"
#include "Utils.h"

#include <cstddef>

extern void potentially_modify_memory(int* a);

int test_aliases(int* a, unsigned int distance, std::size_t n)
{
  int sum = 0;
  for (volatile std::size_t i = 0; i < n; i++) {
    if (i % distance == 0) {
      a[i] = 2;
      potentially_modify_memory(a);// This is done to ensure the compiler does not forward this store to the load below
    }
    sum += a[i];
  }
  return sum;
}

int test_aliases_cloned(int* a, unsigned int distance, std::size_t n)
{
  int sum = 0;
  for (volatile std::size_t i = 0; i < n; i++) {
    if (i % distance == 0) {
      a[i] = 2;
      potentially_modify_memory(a);
    }
    if (i % distance == 0) {
      sum += a[i];
    } else {
      sum += a[i];
    }
  }
  return sum;
}