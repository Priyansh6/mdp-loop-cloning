#include <array>

std::array<int, 1000> l;
std::array<int, 1000> k;

void test_aliases(int* a, int* b, std::size_t n)
{
  for (std::size_t i = 0; i < n; i++) {
    a[i] = b[i];
    b[i] += 1;
  }
}

void test_aliases_cloned(int* a, int* b, std::size_t n)
{
  if (a == b) {
    for (std::size_t i = 0; i < n; i++) {
      a[i] = b[i];
      b[i] += 1;
    }
  } else {
    for (std::size_t i = 0; i < n; i++) {
      a[i] = b[i];
      b[i] += 1;
    }
  }
}

int main()
{
  test_aliases(&l.front(), &k.front(), 1000);
  test_aliases(&l.front(), &l.front(), 1000);

  test_aliases_cloned(&l.front(), &k.front(), 1000);
  test_aliases_cloned(&l.front(), &l.front(), 1000);
  return 0;
}