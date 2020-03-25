#include <vector>
#include <chrono>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <iostream>
#include <unistd.h>

#include "inc/driver/dataset.hpp"
#include "inc/driver/directory.hpp"

#include "inc/sort/introsort.hpp"
#include "inc/sort/quicksort.hpp"
#include "inc/sort/mergesort_ll.hpp"

/* compile with --std=c++11 -D INSTANTIATE_TEMPLATE_TYPE=int */

typedef std::chrono::nanoseconds TimeUnit;

/* ____ introsort quicksort mergesort ____ ____ std::sort unsorted */
typedef unsigned char SortActionMask;

struct ProgramArgs {
	/* Force generate new dataset (overwrite if exists) and exit.
	 * Dataset will be generated according to the problem description.
	 */
	bool regenerate_dataset;  /* -g */

	/* Run single test on one (-S examplesort) or all (-s) algorithms,
	 * compare results to std::sort and exit.
	 */
	std::size_t arr_len;      /* -n */
	int upper_bound;          /* -m */
	bool single_test_all;     /* -s */
	std::string single_test;  /* -S */

	/* Run normal test using given file for storing progress (CSV format).
	 * If given file already contains results, corresponding tests will be skipped.
	 * If file does not exist, new one will be created.
	 */
	std::string filename;     /* -f */
};

struct TestResult {
	std::string filename;
	std::size_t arr_len;
	double sort_level;
	TimeUnit::rep mergesort_time;
	TimeUnit::rep quicksort_time;
	TimeUnit::rep introsort_time;
};

std::ostream& operator<<(std::ostream& stream, const TestResult& tr) {
	return stream << tr.filename << ", "
	              << tr.arr_len << ", "
	              << tr.sort_level << ", "
	              << tr.mergesort_time << ", "
	              << tr.quicksort_time << ", "
	              << tr.introsort_time << '\n';
}

std::istream& operator>>(std::istream& stream, TestResult& tr) {
	char comma;

	stream >> tr.filename;
	comma = tr.filename.back();
	tr.filename.pop_back();
	if (comma != ',')   stream.setstate(std::ios::failbit);
	if (!stream.good()) return stream;

	stream >> tr.arr_len >> comma;
	if (comma != ',')   stream.setstate(std::ios::failbit);
	if (!stream.good()) return stream;

	stream >> tr.sort_level >> comma;
	if (comma != ',')   stream.setstate(std::ios::failbit);
	if (!stream.good()) return stream;

	stream >> tr.mergesort_time >> comma;
	if (comma != ',')   stream.setstate(std::ios::failbit);
	if (!stream.good()) return stream;

	stream >> tr.quicksort_time >> comma;
	if (comma != ',')   stream.setstate(std::ios::failbit);
	if (!stream.good()) return stream;

	stream >> tr.introsort_time;

	return stream;
}

void skip_line(std::istream& stream) {
	while (stream.good() && stream.get() != '\n');
}

class ExitRequired: public std::exception {
	const char* _message;
	int _exit_code;

public:
	ExitRequired(int exit_code, const char* message = nullptr): _exit_code(exit_code), _message(message) { }
	int exit_code() const { return _exit_code; }
	const char* what() const noexcept override { return _message; }
};

void print_usage(const char* progname) {
	std::cout << "usage: "
	          << progname
	          << " [-g] [(-s | -S examplesort) [-n length] [-m max]] [-f filename] [-h]"
	          << std::endl;
}

void print_help(const char* progname) {
	print_usage(progname);
	std::cout << std::endl;
	std::cout << "This program was created to test my implementations of sorting algorithms." << std::endl;
	std::cout << "With no options given this program will ... do nothing." << std::endl;
	std::cout << std::endl;
	std::cout << "Options summary" << std::endl;
	std::cout << "    -g  Force generate new dataset and store it to dataset/ folder." << std::endl;
	std::cout << "        If dataset already exists, it will be overwritten." << std::endl;
	std::cout << "    -s  Run single test of all of my algorithms and std::sort" << std::endl;
	std::cout << "        on random generated dataset and print sorted arrays." << std::endl;
	std::cout << "    -S  Same as -s, but runs test of one given algorithm." << std::endl;
	std::cout << "        Possible arguments are 'mergesort', 'quicksort' and 'introsort'." << std::endl;
	std::cout << "    -n  Number of elements to be generated for single test. Default: 20" << std::endl;
	std::cout << "    -m  Upper bound for single test dataset generator." << std::endl;
	std::cout << "        All the numbers will be from [0 : m-1]. Default: 100" << std::endl;
	std::cout << "    -f  Runs normal test using dataset from dataset/ folder." << std::endl;
	std::cout << "        If dataset does not exist, new one will be created." << std::endl;
	std::cout << "        Progress will be stored to given file in CSV format." << std::endl;
	std::cout << "        If file is not empty, tests which are already passed will be skipped." << std::endl;
	std::cout << "        If file does not exist, new one will be created." << std::endl;
	std::cout << "    -h  Display this help." << std::endl;
}

void print_err(const char* progname, const char* message) {
	std::cerr << progname << ": " << message << std::endl;
}

int optarg2i() {
	int i = std::atoi(optarg);
	if (!i)
		throw ExitRequired(1, "invalid integer argument");
	return i;
}

void parse_args(ProgramArgs& pa, int argc, char* const argv[]) {
	int opt;
	bool n_changed = false, m_changed = false;

	pa.regenerate_dataset = false;
	pa.arr_len = 20;
	pa.upper_bound = 100;
	pa.single_test_all = false;
	pa.single_test = "";
	pa.filename = "";

	while ((opt = getopt(argc, argv, "gn:m:sS:f:h?")) != -1) {
		switch (opt) {
			case 'g':
				pa.regenerate_dataset = true;
				break;
			case 'n':
				n_changed = true;
				// check if > 1
				pa.arr_len = optarg2i();
				break;
			case 'm':
				m_changed = true;
				// check if > 1
				pa.upper_bound = optarg2i();
				break;
			case 's':
				pa.single_test_all = true;
				break;
			case 'S':
				pa.single_test = optarg;
				break;
			case 'f':
				pa.filename = optarg;
				break;
			case 'h':
				print_help(argv[0]);
				throw ExitRequired(0);
			case '?':
				print_usage(argv[0]);
				throw ExitRequired(0);
			default:
				break;
		}
	}

	if (pa.single_test_all && pa.single_test.length())
		throw ExitRequired(2, "conflicting options: -s and -S");

	if (pa.single_test_all && pa.filename.length())
		throw ExitRequired(2, "conflicting options: -s and -f");

	if (pa.single_test.length() && pa.filename.length())
		throw ExitRequired(2, "conflicting options: -S and -f");

	if (pa.arr_len < 2)
		throw ExitRequired(3, "invalid argument: -n");

	if (pa.upper_bound < 2)
		throw ExitRequired(3, "invalid argument: -m");

	if (pa.single_test.length()) {
		if (pa.single_test != "mergesort" && pa.single_test != "quicksort" && pa.single_test != "introsort")
			throw ExitRequired(3, "invalid argument: -S");
	}

	if (!(pa.regenerate_dataset || pa.single_test.length() || pa.single_test_all || pa.filename.length())) {
		print_err(argv[0], "no tasks to be done");
		print_usage(argv[0]);
		throw ExitRequired(0);
	}

	if (!(pa.single_test.length() || pa.single_test_all)) {
		if (n_changed) print_err(argv[0], "warning: unused option: -n");
		if (m_changed) print_err(argv[0], "warning: unused option: -m");
	}
}

void prepare_dataset(Directory& dataset_dir, bool force = false) {
	auto status = dataset_dir.status();
	if (status == Directory::NOT_A_DIRECTORY)
		throw ExitRequired(10, "dataset path is not a directory");
	else if (status == Directory::EXISTS) {
		if (!force)
			return;
		else
			dataset_dir.remove_all();
	}

	std::cout << "Generating dataset...";
	std::cout.flush();

	dataset_dir.create();

	std::size_t each = 100;
	std::vector<std::size_t> sizes = {10000, 50000, 100000, 500000, 1000000};
	std::vector<double> sort_levels = {0, 25, 50, 75, 95, 99, 99.7, -100};

	for (auto size: sizes)
	for (auto sort_level: sort_levels) {
		std::ostringstream stream;
		stream << size << '_' << sort_level;
		Directory dir(dataset_dir.sub(stream.str()));
		dir.create();

		for (int i = 0; i < each; i++) {
			Dataset* dat = Dataset::random(size);
			dat->sort(sort_level);
			dat->save(dir.sub(std::to_string(i+1) + ".dataset"));
			delete dat;
		}

		std::cout << '.';
		std::cout.flush();
	}

	std::cout << " Done!\n";
}

std::chrono::nanoseconds measure(SortActionMask action, Dataset* dat, int** sorted_ptr = nullptr) {
	int* internal_arr = nullptr;
	std::chrono::steady_clock::time_point start, end;

	if (!sorted_ptr)
		sorted_ptr = &internal_arr;

	if (action & 0x02) {
		*sorted_ptr = dat->copy();
		start = std::chrono::steady_clock::now();
		std::sort(*sorted_ptr, *sorted_ptr + dat->length());
		end = std::chrono::steady_clock::now();
	}

	else if (action & 0x10) {
		LLElem<int>* ll = dat->linkedlist();
		LLElem<int>* ll_beginning = ll;
		start = std::chrono::steady_clock::now();
		mergesort_ll(&ll);
		end = std::chrono::steady_clock::now();
		*sorted_ptr = ll_to_array(ll);
		delete[] ll_beginning;
	}

	else if (action & 0x20) {
		*sorted_ptr = dat->copy();
		start = std::chrono::steady_clock::now();
		quicksort(*sorted_ptr, dat->length());
		end = std::chrono::steady_clock::now();
	}

	else if (action & 0x40) {
		*sorted_ptr = dat->copy();
		start = std::chrono::steady_clock::now();
		introsort(*sorted_ptr, dat->length());
		end = std::chrono::steady_clock::now();
	}

	if (internal_arr)
		delete[] internal_arr;

	return std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
}

void print_array(const int* arr, std::size_t length, std::size_t width) {
	std::streamsize prev_width = std::cout.width(width);
	for (std::size_t i = 0; i < length; i++)
		std::cout << arr[i] << ' ';
	std::cout << '\n';
	std::cout.width(prev_width);
}

void single_test(const std::string& algorithm, std::size_t arr_len, int upper_bound) {
	Dataset* dat = Dataset::random(arr_len, upper_bound);
	std::size_t int_print_width = std::to_string(upper_bound-1).length();

	int* sorted;
	SortActionMask action = 0x73;

	if (algorithm.length()) {
		if (algorithm == "introsort")
			action &= 0x4F;
		else if (algorithm == "quicksort")
			action &= 0x2F;
		else if (algorithm == "mergesort")
			action &= 0x1F;
	}

	if (action & 0x01) {
		std::cout << "Generated: ";
		print_array(dat->array(), arr_len, int_print_width);
	}

	if (action & 0x02) {
		measure(0x02, dat, &sorted);
		std::cout << "std::sort: ";
		print_array(sorted, arr_len, int_print_width);
		delete[] sorted;
	}

	if (action & 0x10) {
		measure(0x10, dat, &sorted);
		std::cout << "mergesort: ";
		print_array(sorted, arr_len, int_print_width);
		delete[] sorted;
	}

	if (action & 0x20) {
		measure(0x20, dat, &sorted);
		std::cout << "quicksort: ";
		print_array(sorted, arr_len, int_print_width);
		delete[] sorted;
	}

	if (action & 0x40) {
		measure(0x40, dat, &sorted);
		std::cout << "introsort: ";
		print_array(sorted, arr_len, int_print_width);
		delete[] sorted;
	}

	delete dat;
}

/* Well, this function is VERY DANGEROUS,
 * but okay as for single-use program
 */

void normal_test(Directory& dataset_dir, const std::string& filename) {
	std::vector<std::string> skip_files;
	std::fstream file(filename);

	if (file.is_open()) {
		skip_line(file);
		while (file.good()) {
			TestResult tr;
			file >> tr;
			if (!file.fail())
				skip_files.push_back(tr.filename);
		}
		file.clear();
	}
	else {
		file.clear();
		file.open(filename, std::ios::out);
		file << "Filename, Length, Sort level, Mergesort time, Quicksort time, Introsort time\n";
	}

	std::size_t counter = 3;
	std::cout << "Running test (there will be a lot of dots)...";
	std::cout.flush();

	auto skip_begin = skip_files.begin();
	auto skip_end = skip_files.end();

	for (std::string filename: dataset_dir.iter("*.dataset")) {
		if(std::find(skip_begin, skip_end, filename) == skip_end) {
			TestResult tr;
			Dataset* dat = new Dataset(filename);

			tr.filename = filename;
			tr.arr_len = dat->length();
			tr.sort_level = dat->sort_level();

			tr.mergesort_time = measure(0x10, dat).count();
			tr.quicksort_time = measure(0x20, dat).count();
			tr.introsort_time = measure(0x40, dat).count();

			delete dat;

			file << tr;
			file.flush();

			std::cout << '.';
			std::cout.flush();
			counter++;
		}
	}

	std::cout << " Done! Printed " << counter << " dots!\n";
}

int main(int argc, char* const argv[]) {
	ProgramArgs pa;
	Directory dataset_dir("dataset");

	try {
		parse_args(pa, argc, argv);

		if (pa.filename.length() || pa.regenerate_dataset)
			prepare_dataset(dataset_dir, pa.regenerate_dataset);

		if (pa.single_test_all || pa.single_test.length())
			single_test(pa.single_test, pa.arr_len, pa.upper_bound);

		if (pa.filename.length())
			normal_test(dataset_dir, pa.filename);
	}
	catch (const ExitRequired& err) {
		if (err.what())
			print_err(argv[0], err.what());
		return err.exit_code();
	}


	return 0;
}