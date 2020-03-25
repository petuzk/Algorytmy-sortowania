#include "inc/sort/heapsort.hpp"

template<typename T>
std::size_t repair_node(T* tree, std::size_t length, bool reverse, std::size_t inode) {
	std::size_t ichild = inode*2 + 1;

	if (ichild >= length)
		return 0;

	T* node = tree + inode;
	T* choosen_child = tree + ichild;

	if (ichild + 1 < length && reverse ^ (*choosen_child < *(choosen_child + 1)))
		choosen_child++;

	T nodeval = *node;
	T childval = *choosen_child;

	if (reverse ^ (childval > nodeval)) {
		*node = childval;
		*choosen_child = nodeval;
		return choosen_child - tree;
	}

	return 0;
}

template<typename T>
bool repair_sort_tree(T* array, std::size_t length, bool reverse, std::size_t inode) {
	std::size_t ichild1 = inode*2 + 1;
	std::size_t ichild2 = ichild1 + 1;

start:
	if (repair_node(array, length, reverse, inode) && inode != 0)
		// this node changed, so update parent and then start again
		return true;

	bool ch1changed = ichild1 < length && repair_sort_tree(array, length, reverse, ichild1);
	bool ch2changed = ichild2 < length && repair_sort_tree(array, length, reverse, ichild2);

	if (ch1changed || ch2changed)
		goto start;

	return false;
}

template<typename T>
void heapsort(T* array, std::size_t length, bool reverse) {
	repair_sort_tree(array, length, reverse);
	// print_data(array, length);

	T last_elem;
	std::size_t ichild, iparent, c = length;

	for (length--; length > 0; length--) {
		last_elem = array[length];
		array[length] = array[0];

		iparent = 0;
		ichild = 1;

		while (ichild < length) {
			if (ichild + 1 < length && reverse ^ (array[ichild] < array[ichild + 1]))
				ichild++;

			if (reverse ^ (array[ichild] < last_elem))
				break;

			array[iparent] = array[ichild];
			iparent = ichild;
			ichild = ichild * 2 + 1;
		}

		array[iparent] = last_elem;
	}
}

#ifdef INSTANTIATE_TEMPLATE_TYPE
template void \
	heapsort<INSTANTIATE_TEMPLATE_TYPE>\
	(INSTANTIATE_TEMPLATE_TYPE* array, std::size_t length, bool reverse);
#endif