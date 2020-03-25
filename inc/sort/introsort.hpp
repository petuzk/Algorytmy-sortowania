#pragma once

// http://www.cs.rpi.edu/~musser/gp/introsort.ps

#include <cstdlib>

#include "inc/sort/heapsort.hpp"
#include "inc/sort/quicksort.hpp"

inline int log2_floor(unsigned long x);

template<typename T>
void introsort_dl(T* array, std::size_t length, int depth_limit, bool reverse = false);

template<typename T>
void introsort(T* array, std::size_t length, bool reverse = false);