#include "Utilities.hpp"

using namespace lc;

const UllValueVector* lc::create_range(const unsigned start, const unsigned stop, const unsigned step)
{
    assert(start <= stop);

    UllValueVector* result = new UllValueVector;

    for (unsigned idx = start; idx < stop; idx += step) {
        result->push_back(idx);
    }

    return result;
}

LCString lc::map_to_str(const std::map<LCString, LCString>& m) 
{
    LCString s;
    for (auto& kv : m) {
        s += kv.first + "=" + kv.second + ",";
    }
    return s.substr(0, s.length() - 1);
}

// Returns random number a <= N <= b
unsigned lc::randint(const unsigned a, const unsigned b)
{
    return (rand() % (b - a + 1) + a);
}
