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

class AnnualIncome : public Filter<AnnualIncome>
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    AnnualIncome(const Arguments& args, unsigned* current = nullptr) : Filter<AnnualIncome>(name, args)
    {
        static const std::vector<FilterValue>* options = create_range(15000, 100000, 20000);
        //static const std::vector<FilterValue>* options = create_range(0, 300000, 25000);
        Filter::initialize(options, current);
    }

    inline FilterValue convert(const std::string& raw_data)
    {
        return (raw_data.empty()) ? 0 : boost::numeric_cast<FilterValue>(boost::lexical_cast<double>(raw_data.c_str()));
    }

    const std::string get_string_value() const
    {
        return ">=" + boost::lexical_cast<std::string>(get_value());
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.annual_income >= self.get_value());
    }

    inline bool apply(const LCLoan& loan) const
    {
        return (loan.annual_income >= get_value());
    }
};

};

#endif // __LC_ANNUAL_INCOME_HPP__
