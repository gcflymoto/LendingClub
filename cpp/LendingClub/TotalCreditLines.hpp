/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_TOTAL_CREDIT_LINES_HPP__
#define __LC_TOTAL_CREDIT_LINES_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class TotalCreditLines : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;

    TotalCreditLines() : Filter()
    {
        static const std::vector<FilterValue>* options = create_range(10, 50, 10);
        Filter::initialize(options);
    }
};

};

#endif // __LC_TOTAL_CREDIT_LINES_HPP__
