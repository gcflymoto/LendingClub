/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__
#define __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class MonthsSinceLastDelinquency : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;

    MonthsSinceLastDelinquency() : Filter()
    {
        static std::vector<FilterValue> options;
        if (options.empty()) {
            options.push_back(12);
            options.push_back(24);
            options.push_back(60);
            options.push_back(84);
        }
        Filter::initialize(&options);
    }
};

};

#endif // __LC_MONTHS_SINCE_LAST_DELINQUENCY_HPP__
