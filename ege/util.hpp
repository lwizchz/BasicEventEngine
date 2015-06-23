#include <string>
#include <fstream>

bool read_file(const char* path, std::string& output) {
	std::ifstream t(path);
	t.seekg(0, std::ios::end);
	size_t size = t.tellg();
	output.reserve(size);
	t.seekg(0);
	t.read(&output[0], size);
	return 0;
}
