#include "inc/driver/dataset.hpp"

Dataset::Dataset(std::size_t length): _sort_level(0), _length(length) {
	if (!length)
		_array = nullptr;
	else
		_array = new int[_length];
}

Dataset::Dataset(const std::string& path) {
	std::ifstream file(path);
	file >> _sort_level >> _length;
	_array = new int[_length];
	for (std::size_t i = 0; i < _length; i++)
		file >> _array[i];
	file.close();
}

Dataset* Dataset::random(std::size_t length, int upper_bound) {
	Dataset* d = new Dataset(length);

	if (upper_bound < 2)
		upper_bound = RAND_MAX;

	for (std::size_t i = 0; i < length; i++)
		d->_array[i] = std::rand() % upper_bound;

	return d;
}

Dataset::~Dataset() {
	delete[] _array;
}

std::size_t Dataset::length() const {
	return _length;
}

double Dataset::sort_level() const {
	return _sort_level;
}

std::size_t Dataset::sorted() const {
	return _sort_level * _length / 100;
}

const int* Dataset::array() const {
	return _array;
}

int* Dataset::copy() const {
	int* arr_copy = new int[_length];
	std::copy(_array, _array + _length, arr_copy);

	return arr_copy;
}

LLElem<int>* Dataset::linkedlist() const {
	return array_to_ll(_array, _length);
}

std::size_t Dataset::sort(double sort_level) {

	bool reverse = sort_level < 0;
	if (reverse)
		sort_level = -sort_level;

	if (sort_level < std::fabs(_sort_level))
		return 0;

	_sort_level = sort_level;
	std::size_t new_sorted = sorted();
	if (reverse)
		std::sort(_array, _array + new_sorted, std::greater<int>());
	else
		std::sort(_array, _array + new_sorted, std::less<int>());

	return new_sorted;
}

void Dataset::save(const std::string& path) {
	std::ofstream file(path);
	file << _sort_level << '\n' << _length << '\n';
	for (std::size_t i = 0; i < _length; i++)
		file << _array[i] << (((i+1) % 20 != 0) ? ' ' : '\n');
	file.close();
}
