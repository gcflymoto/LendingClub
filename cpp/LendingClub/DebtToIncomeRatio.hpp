/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_DEBT_TO_INCOME_RATIO_HPP__
#define __LC_DEBT_TO_INCOME_RATIO_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class DebtToIncomeRatio : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    DebtToIncomeRatio() : Filter(name)
    {
        static const FilterValueVector* options = create_range(10 * 100, 45 * 100, 5 * 100);
        Filter::initialize(options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        // Convert DTI 19.48 into normalized 1948
        LCString data;   
        if (raw_data[raw_data.length() - 1] == '%') {
            data = raw_data.substr(0, raw_data.length() - 1);
        }
        else {
            data = raw_data;
        }
        return boost::numeric_cast<FilterValue>(strtod(data.c_str(), nullptr) * 100);
    }

    virtual const LCString get_string_value() const
    {
        return "<=" + boost::lexical_cast<LCString>(boost::numeric_cast<double>(get_value()) / 100);
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.debt_to_income_ratio <= get_value());
    }
};

};

#endif // __LC_DEBT_TO_INCOME_RATIO_HPP__
