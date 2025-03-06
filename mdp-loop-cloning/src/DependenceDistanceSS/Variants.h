#ifndef DEPENDENCE_DISTANCE_SS_VARIANTS_H
#define DEPENDENCE_DISTANCE_SS_VARIANTS_H

#include <cstddef>

int test_base(int* a, unsigned int distance, std::size_t n);
int test_cloned_load(int* a, unsigned int distance, std::size_t n);

#endif// DEPENDENCE_DISTANCE_SS_VARIANTS_H