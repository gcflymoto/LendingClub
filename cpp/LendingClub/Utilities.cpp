#include "Utilities.hpp"

const std::vector<unsigned>* lc::create_range(const unsigned start, const unsigned stop, const unsigned step)
{
	assert(start <= stop);

	std::vector<unsigned>* result = new std::vector<unsigned>;

	for (unsigned idx = start; idx < stop; idx += step) {
		result->push_back(idx);
	}

	return result;
}

std::string lc::map_to_str(const std::map<std::string, std::string>& m) 
{
	std::string s;
	for (auto& kv : m) {
		s += kv.first + "=" + kv.second + ",";
	}
	return s.substr(0, s.length() - 1);
}