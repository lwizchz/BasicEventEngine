#include <sstream>

std::string debug_indent(std::string input, int amount) {
	input.pop_back();
	std::istringstream input_stream (input);
	std::string output;
	while (!input_stream.eof()) {
		std::string tmp;
		getline(input_stream, tmp);
		tmp.append("\n");
		output.append(amount, '\t');
		output.append(tmp);
	}
	return output;
}
