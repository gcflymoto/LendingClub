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
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    AnnualIncome() : Filter(name)
    {
        static const FilterValueVector* options = create_range(0, 300000, 25000);
        Filter::initialize(options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        return (raw_data.empty()) ? 0 : boost::numeric_cast<FilterValue>(boost::lexical_cast<double>(raw_data.c_str()));
    }

    virtual const LCString get_string_value() const
    {
        return ">=" + boost::lexical_cast<LCString>(get_value());
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.annual_income >= get_value());
    }
};

};

#endif // __LC_ANNUAL_INCOME_HPP__
