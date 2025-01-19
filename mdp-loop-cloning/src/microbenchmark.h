#ifndef MICROBENCHMARK_H
#define MICROBENCHMARK_H

#include <cstddef>

int test_aliases(int* a, int* b, std::size_t n);
int test_aliases_cloned(int* a, int* b, std::size_t n);

#endif// MICROBENCHMARK_H