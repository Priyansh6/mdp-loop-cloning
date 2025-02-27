#include "Utils.h"

int redundant_computation(int x)
{
  int sum = 0;
  for (int i = 0; i < x; i++) { sum += i; }
  return sum;
}

void potentially_modify_memory([[maybe_unused]] int* a) { return; }