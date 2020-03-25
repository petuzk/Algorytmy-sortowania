#include "inc/sort/mergesort_ll.hpp"

template<typename T>
void mergesort_ll(LLElem<T>** llptr, bool reverse) {
	for (std::size_t groupsize = 1; ; groupsize <<= 1) {
		LLElem<T>* tmpgroup;
		LLElem<T>* buildergroup = *llptr;
		LLElem<T>* othergroup = *llptr;
		LLElem<T>** builder = llptr;

		while (buildergroup) {
			for (std::size_t i = 0; othergroup && i < groupsize; i++) {
				othergroup = othergroup->next;
			}

			if (!othergroup)
				break;

			std::size_t tmpsize, swap_count = 0;
			std::size_t buildergroupsize = groupsize, othergroupsize = groupsize;

			while (buildergroupsize) {
				if (reverse ^ (othergroup->obj < buildergroup->obj)) {
					*builder = othergroup;
					// swap groups
					tmpgroup = othergroup->next;
					othergroup = buildergroup;
					buildergroup = tmpgroup;
					// swap sizes
					tmpsize = othergroupsize;
					othergroupsize = buildergroupsize;
					buildergroupsize = tmpsize;
					swap_count++;
				}
				else {
					buildergroup = buildergroup->next;
				}

				if (!buildergroup) buildergroupsize = 0;
				else buildergroupsize--;

				builder = &((*builder)->next);
			}

			*builder = othergroup;

			for (; othergroup && othergroupsize; othergroupsize--) {
				othergroup = othergroup->next;
				builder = &((*builder)->next);
			}

			if (swap_count % 2 == 0)
				*builder = buildergroup = othergroup;
			else
				*builder = othergroup = buildergroup;
		}

		if (builder == llptr)
			return;
	}
}

#ifdef INSTANTIATE_TEMPLATE_TYPE
template void \
	mergesort_ll<INSTANTIATE_TEMPLATE_TYPE>\
	(LLElem<INSTANTIATE_TEMPLATE_TYPE>** llptr, bool reverse);
#endif