#include "LoopClone/Variants.h"
#include "Utils.h"

#include <cstddef>
#include <cstdint>

int test_base(int* a, int* b, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    a[slow_identity(i)] = 2;
    sum += b[i];
  }
  return sum;
}

int test_cloned_load(int* a, int* b, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    a[slow_identity(i)] = 2;
    if (&a[i] == &b[i]) {
      sum += b[i];
    } else {
      sum += b[i];
    }
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

int test_cloned_loop_with_overlap_check(int* a, int* b, std::size_t n)
{
  uintptr_t startA = reinterpret_cast<uintptr_t>(a);
  uintptr_t endA = startA + n * sizeof(*a);
  uintptr_t startB = reinterpret_cast<uintptr_t>(b);
  uintptr_t endB = startB + n * sizeof(*b);

  volatile bool nonOverlap = (endA <= startB) || (endB <= startA);
  int sum = 0;
  if (nonOverlap) {
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