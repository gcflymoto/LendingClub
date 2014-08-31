/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_DELINQUENCIES_HPP__
#define __LC_DELINQUENCIES_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class Delinquencies : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;

    Delinquencies() : Filter()
    {
        static std::vector<FilterValue> options;
        if (options.empty()) {
            options.push_back(1 << 0);                                      // 0
            options.push_back(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3);           // 0 - 3
            options.push_back(1 << 0 | 1 << 1 | 1 << 2 | 1 << 3 | 1 << 4);  // 0 - 4
            options.push_back(1 << 4);                                      // 4
            options.push_back(1 << 5 | 1 << 6 | 1 << 7 | 1 << 8 | 1 << 9 | 1 << 10 | 1 << 11); // 5 - 11
        }

        Filter::initialize(&options);
    }
};

};

#endif // __LC_DELINQUENCIES_HPP__
