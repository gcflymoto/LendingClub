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
    static const LCString sqlite_type;
    static const LCString csv_name;
    static const LCString name;

    TotalCreditLines() : Filter(name)
    {
        static const FilterValueVector* options = create_range(10, 50, 10);
        Filter::initialize(options);
    }

    virtual FilterValue convert(const LCString& raw_data) const
    {
        return (raw_data.empty()) ? 0 : boost::lexical_cast<FilterValue>(raw_data.c_str());
    }

    virtual const LCString get_string_value() const
    {
        return "<=" + boost::lexical_cast<LCString>(get_value());
    }

    static bool static_apply(const Filter& self, const Loan& loan)
    {
        return (loan.total_acc <= self.get_value());
    }

    inline bool apply(const Loan& loan) const
    {
        return (loan.total_acc <= get_value());
    }

    virtual const LCString& get_name() const
    {
        return name;
    }
};

};

#endif // __LC_TOTAL_CREDIT_LINES_HPP__
