/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_ANNUAL_INCOME_HPP__
#define __LC_ANNUAL_INCOME_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class AnnualIncome : public Filter
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;

    AnnualIncome() : Filter()
    {
        static const std::vector<FilterValue>* options = create_range(0, 300000, 25000);
        Filter::initialize(options);
    }
};

};

#endif // __LC_ANNUAL_INCOME_HPP__
