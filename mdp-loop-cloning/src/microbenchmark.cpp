#include "microbenchmark.h"

#include <cstddef>

int test_aliases(int* a, int* b, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    a[i] = 2;
    sum += b[i];
  }
  return sum;
}

int test_aliases_cloned(int* a, int* b, std::size_t n)
{
  int sum = 0;
  if (a == b) {
    for (std::size_t i = 0; i < n; i++) {
      a[i] = 2;
      sum += b[i];
    }
  } else {
    for (std::size_t i = 0; i < n; i++) {
      a[i] = 2;
      sum += b[i];
    }
  }
  return sum;
}