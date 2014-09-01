/*
Created on July 28, 2014

@author:     Gregory Czajkowski

@copyright:  2013 Freedom. All rights reserved.

@license:    Licensed under the Apache License 2.0 http://www.apache.org/licenses/LICENSE-2.0

@contact:    gregczajkowski at yahoo.com
*/

#ifndef __LC_INCOME_VALIDATED_HPP__
#define __LC_INCOME_VALIDATED_HPP__

#include "Filter.hpp"
#include "Loan.hpp"
#include "Utilities.hpp"

namespace lc
{

class IncomeValidated : public Filter
{
public:
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    IncomeValidated() : Filter(name)
    {
        static const FilterValueVector* options = create_range(0, 2, 1);
        Filter::initialize(options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        if (raw_data == "TRUE") {
            return 1;
        }
        else {
            return 0;
        }
    }

    virtual const LCString get_string_value() const
    {
        auto value = get_value();
        if (value == 0) {
            return "income validated";
        }
        else {
            return "income unvalidated";
        }
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.income_validated != get_value());
    }
};

};

#endif // __LC_INCOME_VALIDATED_HPP__
