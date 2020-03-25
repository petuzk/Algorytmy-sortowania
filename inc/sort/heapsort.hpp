#pragma once

#include <cstdlib>

template<typename T>
std::size_t repair_node(T* tree, std::size_t length, bool reverse, std::size_t inode);

template<typename T>
bool repair_sort_tree(T* array, std::size_t length, bool reverse, std::size_t inode = 0);

template<typename T>
void heapsort(T* array, std::size_t length, bool reverse = false);