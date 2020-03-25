#pragma once

#include <string>
#include <cstdio>
#include <cstdlib>

#define _BSD_SOURCE
#include <stdio.h>

class Directory {
	std::string _path;

	class Iterator {
		std::size_t _index;
		std::string _files;

	public:
		Iterator();
		Iterator(const std::string& files);
		const Iterator& begin() const;
		Iterator end() const;
		const Iterator& operator++();
		bool operator!=(const Iterator& other) const;
		bool is_end() const;
    std::string operator*() const;
	};

public:
	enum Status {
		DOES_NOT_EXIST,
		NOT_A_DIRECTORY,
		EXISTS
	};

	Directory(const std::string& path);

	const std::string& path() const;
	std::string sub(const std::string& path) const;

	int create() const;
	Status status() const;
	int remove_all() const;
	Iterator iter(const char* file_search_pattern) const;
};