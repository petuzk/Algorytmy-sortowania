#include "inc/sort/quicksort.hpp"

template<typename T>
T* quicksort_partition(T* array, std::size_t length, bool reverse) {
	return array + length / 2 - 1;
}

template<typename T>
T* quicksort_body(T* array, std::size_t length, bool reverse, T* pivot) {
	T tmp;
	T* end = array + length - 1;
	T* li = array;
	T* ri = end;
	T* pivot_place = nullptr;

	while (true) {
		while ((reverse ^ (*li <= *pivot) || li == pivot) && li < end  ) li++;
		while ((reverse ^ (*ri >  *pivot) || ri == pivot) && ri > array) ri--;

		if (li >= ri) break;

		tmp = *li;
		*li = *ri;
		*ri = tmp;
	}

	if (pivot > li) pivot_place = li;
	else if (pivot < ri) pivot_place = ri;

	if (pivot_place) {
		tmp = *pivot;
		*pivot = *pivot_place;
		*pivot_place = tmp;
		pivot = pivot_place;
	}

	return pivot;
}

template<typename T>
void quicksort(T* array, std::size_t length, bool reverse) {
	if (length < 2)
		return;

	T* pivot = quicksort_partition(array, length, reverse);
	pivot = quicksort_body(array, length, reverse, pivot);

	quicksort(array, pivot - array, reverse);
	quicksort(pivot + 1, array + length - pivot - 1, reverse);
}

#ifdef INSTANTIATE_TEMPLATE_TYPE
template void \
	quicksort<INSTANTIATE_TEMPLATE_TYPE>\
	(INSTANTIATE_TEMPLATE_TYPE* array, std::size_t length, bool reverse);
#endif