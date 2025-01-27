#include "SimpleSS/Variants.cpp"

#include <array>
#include <cstddef>

#define CLONE 1

constexpr std::size_t N = 1000000;

std::array<int, N> l;
std::array<int, N> k;

int main()
{
  int sum = 0;

#if CLONE
  sum += test_aliases_cloned(&l.front(), &l.front(), 10);
  sum += test_aliases_cloned(&l.front(), &k.front(), N);
#else
  sum += test_aliases(&l.front(), &l.front(), 10);
  sum += test_aliases(&l.front(), &k.front(), N);
#endif

  return sum;
}