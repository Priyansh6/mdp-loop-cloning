#include "SimpleSS/Variants.h"
#include "Utils.h"

#include <cstddef>

int test_base(int* a, int* b, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    a[slow_identity(i)] = 2;
    sum += b[i];
  }
  return sum;
}

int test_cloned_loop(int* a, int* b, std::size_t n)
{
  int sum = 0;
  if (a == b) {
    for (std::size_t i = 0; i < n; i++) {
      a[slow_identity(i)] = 2;
      sum += b[i];
    }
  } else {
    for (std::size_t i = 0; i < n; i++) {
      a[slow_identity(i)] = 2;
      sum += b[i];
    }
  }
  return sum;
}