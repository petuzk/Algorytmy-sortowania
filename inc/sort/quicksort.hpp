#pragma once

#include <cstdlib>

template<typename T>
T* quicksort_partition(T* array, std::size_t length, bool reverse);

template<typename T>
T* quicksort_body(T* array, std::size_t length, bool reverse, T* pivot);

template<typename T>
void quicksort(T* array, std::size_t length, bool reverse = false);