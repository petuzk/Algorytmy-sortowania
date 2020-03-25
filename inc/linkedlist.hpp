#pragma once

#include <cstdlib>

template<typename T>
struct LLElem {
	T obj;
	LLElem<T>* next;
};

template<typename T>
LLElem<T>* array_to_ll(const T* array, std::size_t length);

template<typename T>
T* ll_to_array(LLElem<T>* ll, std::size_t* length = nullptr);