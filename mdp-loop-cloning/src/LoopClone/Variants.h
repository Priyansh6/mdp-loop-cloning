#ifndef LOOP_CLONE_VARIANTS_H
#define LOOP_CLONE_VARIANTS_H

#include <cstddef>

int test_base(int* a, int* b, std::size_t n);
int test_cloned_load(int* a, int* b, std::size_t n);
int test_cloned_loop(int* a, int* b, std::size_t n);
int test_cloned_loop_with_overlap_check(int* a, int* b, std::size_t n);

#endif// LOOP_CLONE_VARIANTS_H