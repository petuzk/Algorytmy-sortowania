#include "inc/driver/directory.hpp"

Directory::Iterator::Iterator(): _index(std::string::npos) { }

Directory::Iterator::Iterator(const std::string& files): _index(0), _files(files) {
	if (_files.find('\n') == std::string::npos)
		_index = std::string::npos;
}

const Directory::Iterator& Directory::Iterator::begin() const {
	return *this;
}

Directory::Iterator Directory::Iterator::end() const {
	return Iterator();
}

const Directory::Iterator& Directory::Iterator::operator++() {
	_index = _files.find('\n', _index);
	if (_index != std::string::npos)
		_index++;
	if (_index == _files.length())
		_index = std::string::npos;

	return *this;
}

bool Directory::Iterator::operator!=(const Directory::Iterator& other) const {
	return _index != other._index;
}

bool Directory::Iterator::is_end() const {
	return _index == std::string::npos;
}

std::string Directory::Iterator::operator*() const {
	if (is_end())
		return "";

	std::size_t eol = _files.find('\n', _index);
	if (eol == std::string::npos)
		return _files.substr(_index);
	else
		return _files.substr(_index, eol - _index);
}



Directory::Directory(const std::string& path) {
	_path = path;
}

const std::string& Directory::path() const {
	return _path;
}

std::string Directory::sub(const std::string& path) const {
	return _path + '/' + path;
}

int Directory::create() const {
	std::string cmd = "mkdir " + _path;
	return std::system(cmd.c_str());
}

Directory::Status Directory::status() const {
	std::string cmd = "test -e " + _path;
	if (std::system(cmd.c_str()))
		return DOES_NOT_EXIST;

	cmd[6] = 'd'; // replace -e with -d
	if (std::system(cmd.c_str()))
		return NOT_A_DIRECTORY;

	return EXISTS;
}

int Directory::remove_all() const {
	std::string cmd = "rm -r " + _path;
	return std::system(cmd.c_str());
}

Directory::Iterator Directory::iter(const char* file_search_pattern) const {
	std::string cmd = "find \"" + _path + "\" -name \"" + file_search_pattern + '"';
	FILE* stream = popen(cmd.c_str(), "r");

	char buffer[64];
	std::size_t read;
	std::string stdout;
	while (!feof(stream) && !ferror(stream)) {
		read = std::fread(buffer, 1, 63, stream);
		buffer[read] = '\0';
		stdout += buffer;
	}

	pclose(stream);
	return Iterator(stdout);
}

