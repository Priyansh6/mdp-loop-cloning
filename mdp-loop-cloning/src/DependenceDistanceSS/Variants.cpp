#include "DependenceDistanceSS/Variants.h"

#include <cstddef>

int test_aliases(int* array, unsigned distance, int* num, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = distance; i < n; i++) {
    if (i % distance == 0) {
      array[i] = 2;
    } else {
      sum += 3;
    }
    sum += array[i];
  }
  return sum;
}

int test_aliases_clone_per_iteration(int* a, unsigned distance, std::size_t n)
{
  for (std::size_t i = distance; i < n; i++) {
    if (i % 2 == 0) {
      a[i] = a[i - distance] * 2;
    } else {
      a[i] = a[i - distance] * 3;
    }
    a[i] = a[i - distance] * 2;
  }
}