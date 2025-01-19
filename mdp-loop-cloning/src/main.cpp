#include "microbenchmark.h"

#include <array>
#include <cstddef>

#define CLONE 1

constexpr std::size_t N = 1000000;

std::array<int, N> l;
std::array<int, N> k;

int main()
{
#if CLONE
  test_aliases_cloned(&l.front(), &l.front(), N);
  test_aliases_cloned(&l.front(), &k.front(), N);
#else
  test_aliases(&l.front(), &l.front(), N);
  test_aliases(&l.front(), &k.front(), N);
#endif

  return 0;
}