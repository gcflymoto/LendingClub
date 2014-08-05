/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_UTILITIES_HPP__
#define __LC_UTILITIES_HPP__

#include <cassert>
#include <vector>
#include <map>
#include <string>

namespace lc {
    const std::vector<unsigned long long>* create_range(const unsigned start, const unsigned stop, const unsigned step);
    std::string map_to_str(const std::map<std::string, std::string>& m);

    // Returns random number a <= N <= b
    unsigned randint(const unsigned a, const unsigned b);
};

#endif // __LC_UTILITIES_HPP__