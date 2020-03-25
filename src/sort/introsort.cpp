#include "inc/sort/introsort.hpp"

inline int log2_floor(unsigned long x) {
	int log = 0;
	while ((x >>= 1))
		log++;
	return log;
}

template<typename T>
void introsort_dl(T* array, std::size_t length, int depth_limit, bool reverse) {
	if (length < 2)
		return;
	else if (!depth_limit) {
		heapsort(array, length, reverse);
		return;
	}

	T* pivot = quicksort_partition(array, length, reverse);
	pivot = quicksort_body(array, length, reverse, pivot);

	--depth_limit;
	introsort_dl(array, pivot - array, depth_limit, reverse);
	introsort_dl(pivot + 1, array + length - pivot - 1, depth_limit, reverse);
}

template<typename T>
void introsort(T* array, std::size_t length, bool reverse) {
	introsort_dl(array, length, 2 * log2_floor(length), reverse);
}

#ifdef INSTANTIATE_TEMPLATE_TYPE
template void \
	introsort<INSTANTIATE_TEMPLATE_TYPE>\
	(INSTANTIATE_TEMPLATE_TYPE* array, std::size_t length, bool reverse);
#endif