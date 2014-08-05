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
    static const std::string name;

    TotalCreditLines(const Arguments& args, unsigned* current = nullptr) : Filter(name, args)
    {
        static const std::vector<FilterValue>* options = create_range(10, 40, 10);
        Filter::initialize(options, current);
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.total_acc <= self.get_value());
    }

    inline bool apply(const LCLoan& loan)
    {
        return (loan.total_acc <= get_value());
    }
};

};

#endif // __LC_TOTAL_CREDIT_LINES_HPP__
