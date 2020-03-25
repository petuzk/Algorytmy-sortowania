#pragma once

#include <cmath>
#include <fstream>
#include <cstdlib>
#include <algorithm>
#include <functional>  // std::greater, std::less

#include "inc/linkedlist.hpp"

class Dataset {
	double _sort_level;
	std::size_t _length;
	int* _array;

	Dataset(std::size_t length);

public:
	Dataset(const std::string& path);
	static Dataset* random(std::size_t length, int upper_bound = 0);

	~Dataset();

	std::size_t length() const;
	double sort_level() const;
	std::size_t sorted() const;

	const int* array() const;
	int* copy() const;
	LLElem<int>* linkedlist() const;

	std::size_t sort(double sort_level);

	void save(const std::string& path);
};