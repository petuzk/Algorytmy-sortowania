#include "inc/linkedlist.hpp"

template<typename T>
LLElem<T>* array_to_ll(const T* array, std::size_t length) {
	LLElem<T>* ll = new LLElem<T>[length];

	for (std::size_t i = 0; i < length; i++) {
		ll[i].obj = array[i];
		ll[i].next = i+1 == length ? nullptr : ll + i + 1;
	}

	return ll;
}

template<typename T>
T* ll_to_array(LLElem<T>* ll, std::size_t* length) {
	std::size_t internal_len;
	if (!length)
		length = &internal_len;

	*length = 0;

	T* array = nullptr;

	for (std::size_t alloc_size = 2; ll; alloc_size <<= 1) {
		array = static_cast<T*>(std::realloc(array, alloc_size * sizeof(T)));
		while (ll && *length < alloc_size) {
			array[(*length)++] = ll->obj;
			ll = ll->next;
		}
	}

	return array;
}

#ifdef INSTANTIATE_TEMPLATE_TYPE
template LLElem<INSTANTIATE_TEMPLATE_TYPE>* \
	array_to_ll<INSTANTIATE_TEMPLATE_TYPE>\
	(const INSTANTIATE_TEMPLATE_TYPE* array, std::size_t length);
template INSTANTIATE_TEMPLATE_TYPE* \
	ll_to_array<INSTANTIATE_TEMPLATE_TYPE>\
	(LLElem<INSTANTIATE_TEMPLATE_TYPE>* ll, std::size_t* length);
#endif