#ifndef UTILS_H
#define UTILS_H

#include <concepts>

int redundant_computation(int x);

template<typename T>
  requires std::convertible_to<T, double>
inline T slow_identity(T x)
{
  double x_d = static_cast<double>(x);
  x_d = x_d + 0.00001;
  x_d = x_d * 2.0;
  x_d = x_d - 0.00001;
  x_d = x_d / 2.0;
  return static_cast<T>(x_d);
};

void potentially_modify_memory([[maybe_unused]] int* a);
void potentially_modify_memory2([[maybe_unused]] int* a);

inline int computeComplexSum(int a, int b)
{
  int temp = a + b;
  temp = temp * (a - b);
  temp = temp ^ (a + temp);
  temp = temp >> 2;
  temp = temp + a * b;
  temp = temp * (a - b);
  temp = temp ^ (a + temp);
  temp = temp >> 2;
  temp = temp + a * b;
  return temp;
}

#endif// UTILS_H