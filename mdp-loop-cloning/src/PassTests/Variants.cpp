#include "PassTests/Variants.h"
#include "Utils.h"

#include <chrono>
#include <cstddef>

// volatile int sink = 0;

// __attribute__((noinline)) void do_some_work(int x)
// {
//   int a = x;
//   a += 1;
//   a *= 3;
//   a ^= 0x55;
//   a |= 0x1234;
//   a = (a << 1) | (a >> 3);
//   sink += a;// Prevent optimization
// }

int test_base(int* a, int* b, std::size_t n)
{
  int sum = 0;
  for (std::size_t i = 0; i < n; i++) {
    a[i] = 2;
    sum += b[i];
    // do_some_work(i);
  }
  return sum;
}

int test_cloned_loop(int* a, int* b, std::size_t n)
{
  int sum = 0;
  if (a == b) {
    for (std::size_t i = 0; i < n; i++) {
      a[i] = 2;
      sum += b[i];
      // do_some_work(i);
    }
  } else {
    for (std::size_t i = 0; i < n; i++) {
      a[i] = 2;
      sum += b[i];
      // do_some_work(i);
    }
  }
  return sum;
}