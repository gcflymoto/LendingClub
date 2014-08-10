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

class IncomeValidated : public Filter<IncomeValidated>
{
public:
    static const std::string sqlite_type;
    static const std::string csv_name;
    static const std::string name;

    IncomeValidated(const Arguments& args, unsigned* current = nullptr) : Filter<IncomeValidated>(name, args)
    {
        static const std::vector<FilterValue>* options = create_range(0, 2, 1);
        Filter::initialize(options, current);
    }

    inline FilterValue convert(const std::string& raw_data)
    {
        return (raw_data == "TRUE") ? 1 : 0;
    }

    const std::string get_string_value() const
    {
        return (get_value() ? "income validated" : "income unvalidated");
    }

    static bool static_apply(const Filter& self, const LCLoan& loan)
    {
        return (loan.income_validated == self.get_value());
    }

    inline bool apply(const LCLoan& loan) const
    {
        return (loan.income_validated == get_value());
    }
};

};

#endif // __LC_INCOME_VALIDATED_HPP__
